/*
 * ZmQML - QML binding for zeromq.
 *
 * Copyright (C) 2014 Riccardo Ferrazzo
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "zmqsocket.h"
#include "zmqcontext.h"

#include <QDebug>
#include <QSocketNotifier>
#include <QUuid>

using namespace std::placeholders;

inline void zmqError(const QString &message) {
    qWarning() << message << zmq_strerror(zmq_errno());
}

ZMQSocket::ZMQSocket(QObject *parent) :
    QObject(parent),
    _connection_status(Invalid),
    _type(Null),
    _identity(QUuid::createUuid().toByteArray()),
    socket(0)
{}

ZMQSocket::~ZMQSocket()
{
    if (socket)
        zmq_close(socket);
}

ZMQSocket::ConnectionStatus ZMQSocket::status() const
{
    return _connection_status;
}

ZMQSocket::SocketType ZMQSocket::type() const
{
    return _type;
}

QByteArray ZMQSocket::identity() const
{
    return _identity;
}

QVariantList ZMQSocket::addresses() const
{
    return _addr;
}

QStringList ZMQSocket::subscriptions() const
{
    return _subscriptions.toList();
}

void ZMQSocket::setType(const ZMQSocket::SocketType type)
{
    if (_type != Null)
        return;

    _type = type;
    emit typeChanged();

    setup();
}

void ZMQSocket::setIdentity(const QByteArray &id)
{
    if (_identity == id)
        return;

    if (socket) {
        int rc = zmq_setsockopt(socket, ZMQ_IDENTITY, (void *) id.data(), id.size());
        if (rc < 0 )
            zmqError("Identity error:");
    }

    _identity = id;
    emit identityChanged();
}

void ZMQSocket::setAddresses(const QVariantList &addresses)
{
    if (socket && _connection_status == Connected) {
        int (*dfun)(void *, const char*) = _connection_method == Connect ? zmq_disconnect : zmq_unbind;

        foreach (const QVariant &a, _addr) {
            int rc = dfun(socket, qPrintable(a.toUrl().toString()));

            if (rc < 0)
                zmqError("Disconnection error:");
        }


        int (*cfun)(void *, const char*) = _connection_method == Connect ? zmq_connect : zmq_bind;
        int result;

        foreach(const QVariant &addr, addresses) {
            result = cfun(socket, qPrintable(addr.toUrl().toString()));

            if (result == -1)
                zmqError("Connection error:");
        }

    }

    _addr = addresses;
    emit addressesChanged();
}

void ZMQSocket::setSubscriptions(const QStringList &sub)
{
    const QSet<QString> new_subs = sub.toSet();
    if (_subscriptions == new_subs)
        return;

    if (socket && _type == Sub) {
        const QSet<QString> removed = _subscriptions - new_subs;
        const QSet<QString> added = new_subs - _subscriptions;

        foreach (const QString &s, removed) {
            const QByteArray &b = s.toUtf8();
            zmq_setsockopt(socket, ZMQ_UNSUBSCRIBE, (void *) b.data(), b.size());
        }

        foreach (const QString &s, added) {
            const QByteArray &b = s.toUtf8();
            zmq_setsockopt(socket, ZMQ_SUBSCRIBE, (void *) b.data(), b.size());
        }
    }

    _subscriptions = new_subs;
    emit subscriptionsChanged();
}

bool ZMQSocket::setSockOption(ZMQSocket::SockOption option, const QVariant &value)
{
    if (!options.size())
        setupOptionsTable();

    if (!socket) {
        qWarning() << "Error, socket not ready";
        return false;
    }

    if (!options.contains(option)) {
        qWarning() << "ZMQSocket::setSockOption: unknown option" << option;
        return false;
    }

    return options[option].setter(value);
}

QVariant ZMQSocket::getSockOption(ZMQSocket::SockOption option)
{
    if (!options.size())
        setupOptionsTable();

    if (!options.contains(option)) {
        qWarning() << "ZMQSocket::getSockOption: unknown option" << option;
        return QVariant();
    }

    return options[option].getter();
}

bool ZMQSocket::connectSocket()
{
    return setupConnection(Connect);
}

bool ZMQSocket::bindSocket()
{
    return setupConnection(Bind);
}

void ZMQSocket::sendMessage(const QByteArray &message)
{
    const int res = zmq_send(socket, message.data(), message.size(), 0);

    if (res == -1) {
        zmqError("Error sending message:");
        return;
    }
}

void ZMQSocket::sendMessage(const QList<QByteArray> &message)
{
    QList<QByteArray>::const_iterator i;
    for (i = message.constBegin(); i != message.constEnd(); ++i) {
        int flags = (i+1) == message.constEnd() ? 0 : ZMQ_SNDMORE;

        const int res = zmq_send(socket, (*i).constData(), (*i).size(), flags);

        if (res == -1) {
            zmqError("Error sending message:");
            return;
        }
    }
}

void ZMQSocket::setup()
{
    if (!_type)
        return;

    socket = zmq_socket(ZMQContext::instance()->context, int(_type));

    if (!socket) {
        zmqError("Socket creation error:");
        return;
    }

    _connection_status = Disconnected;
    emit connectionStatusChanged();

    zmq_setsockopt(socket, ZMQ_IDENTITY, (void *) _identity.data(), _identity.size());

    qint32 fd;
    size_t size = sizeof(fd);
    zmq_getsockopt(socket, ZMQ_FD, &fd, &size);

    notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);

    connect(notifier, &QSocketNotifier::activated,
        [=]() {
            notifier->setEnabled(false);

            quint32 event = ZMQ_POLLIN;
            QList<QByteArray> message;

            while(event & ZMQ_POLLIN){
                size_t ev_size = sizeof(event);
                zmq_getsockopt(socket, ZMQ_EVENTS, (void *) &event, &ev_size);

                zmq_msg_t part;
                int rc = zmq_msg_init(&part);
                if (rc < 0) {
                    zmqError("Error initializing message:");
                    break;
                }

                const int res = zmq_msg_recv(&part, socket, ZMQ_NOBLOCK);

                if (res < 0)
                    continue;

                message << QByteArray((char *) zmq_msg_data(&part), zmq_msg_size(&part));

                zmq_msg_close(&part);

                qint64 more = 0;
                size_t size = sizeof(more);
                zmq_getsockopt(socket, ZMQ_RCVMORE, static_cast<void *>(&more), &size);

                if (!more && message.length()) {
                    emit messageReceived(message);
                    message.clear();
                }
            }

            if (message.length() > 0)
                emit messageReceived(message);

            notifier->setEnabled(true);
        }
    );
}

bool ZMQSocket::setupConnection(ConnectionMethod method)
{
    if (_connection_status != Disconnected) {
        qWarning() << QString("Socket is not ready to %1").arg(method == Connect ? "connect" : "bind");
        return false;
    }

    int (*fun)(void *, const char *) = method == Connect ? zmq_connect : zmq_bind;
    int result;

    foreach(const QVariant &addr, _addr) {
        result = fun(socket, qPrintable(addr.toUrl().toString()));

        if (result == -1) {
            zmqError("Connection error:");
            return false;
        }
    }

    _connection_method = method;
    _connection_status = Connected;
    emit connectionStatusChanged();

    return true;
}

void ZMQSocket::setupOptionsTable()
{
    if (options.size())
        return;

    auto setIntValue = [this] (SockOption key, const QVariant &value) {
        bool ok;
        int int_val = value.toInt(&ok);
        if (!ok) {
            qWarning() << "Value is not int";
            return false;
        }

        const int rc = zmq_setsockopt(socket, static_cast<int>(key), static_cast<void *>(&int_val), sizeof(int));
        if (rc != 0) {
            zmqError(QString("Error setting option %1:").arg(key));
            return false;
        }

        return true;
    };

    auto getIntValue = [this] (SockOption key) {
        int option;
        size_t size = sizeof(option);

        const int rc = zmq_getsockopt(socket, static_cast<int>(key), static_cast<void *>(&option), &size);
        if (rc != 0) {
            zmqError(QString("Error getting option %1:").arg(key));
            return QVariant();
        }

        return QVariant(option);
    };

    auto setByteArrayValue = [this] (SockOption key, const QVariant &value) {
        QByteArray ba_value = value.toByteArray();
        if (ba_value.isNull()) {
            qWarning() << "Value is not a QByteArray";
            return false;
        }

        const int rc = zmq_setsockopt(socket, static_cast<int>(key), ba_value.data(), ba_value.size());
        if (rc != 0) {
            zmqError(QString("Error setting option %1:").arg(key));
            return false;
        }

        return true;
    };


    auto getByteArrayValue = [this] (SockOption key, size_t size) {
        QByteArray data(size, ' ');

        const int rc = zmq_getsockopt(socket, static_cast<int>(key), static_cast<void *>(data.data()), &size);
        if (rc != 0) {
            zmqError(QString("Error getting option %1:").arg(key));
            return QVariant();
        }

        data.resize(data.indexOf('\0'));

        return QVariant(data);
    };

    options = QHash<SockOption, Option>({
        {SndHwm, {std::bind(setIntValue, SndHwm, _1), std::bind(getIntValue, SndHwm)}},
        {RcvHwm, {std::bind(setIntValue, RcvHwm, _1), std::bind(getIntValue, RcvHwm)}},
        {Rate, {std::bind(setIntValue, Rate, _1), std::bind(getIntValue, Rate)}},
        {RecoveryIvl, {std::bind(setIntValue, RecoveryIvl, _1), std::bind(getIntValue, RecoveryIvl)}},
        {SndBuf, {std::bind(setIntValue, SndBuf, _1), std::bind(getIntValue, SndBuf)}},
        {RcvBuf, {std::bind(setIntValue, RcvBuf, _1), std::bind(getIntValue, RcvBuf)}},
        {Linger, {std::bind(setIntValue, Linger, _1), std::bind(getIntValue, Linger)}},
        {ReconnectIvl, {std::bind(setIntValue, ReconnectIvl, _1), std::bind(getIntValue, ReconnectIvl)}},
        {ReconnectIvlMax, {std::bind(setIntValue, ReconnectIvlMax, _1), std::bind(getIntValue, ReconnectIvlMax)}},
        {Backlog, {std::bind(setIntValue, Backlog, _1), std::bind(getIntValue, Backlog)}},
        {MulticastHops, {std::bind(setIntValue, MulticastHops, _1), std::bind(getIntValue, MulticastHops)}},
        {SndTimeOut, {std::bind(setIntValue, SndTimeOut, _1), std::bind(getIntValue, SndTimeOut)}},
        {IPV4Only, {std::bind(setIntValue, IPV4Only, _1), std::bind(getIntValue, IPV4Only)}},
        {RouterMandatory, {std::bind(setIntValue, RouterMandatory, _1), std::bind(getIntValue, RouterMandatory)}},
        {XPubVerbose, {std::bind(setIntValue, XPubVerbose, _1), std::bind(getIntValue, XPubVerbose)}},
        {TcpKeepalive, {std::bind(setIntValue, TcpKeepalive, _1), std::bind(getIntValue, TcpKeepalive)}},
        {TcpKeepaliveIdle, {std::bind(setIntValue, TcpKeepaliveIdle, _1), std::bind(getIntValue, TcpKeepaliveIdle)}},
        {TcpKeepaliveCnt, {std::bind(setIntValue, TcpKeepaliveCnt, _1), std::bind(getIntValue, TcpKeepaliveCnt)}},
        {TcpKeepaliveIntvl, {std::bind(setIntValue, TcpKeepaliveIntvl, _1), std::bind(getIntValue, TcpKeepaliveIntvl)}},
#if ZMQ_VERSION_MAJOR > 3
        {IPV6, {std::bind(setIntValue, IPV6, _1), std::bind(getIntValue, IPV6)}},
        {Immediate, {std::bind(setIntValue, Immediate, _1), std::bind(getIntValue, Immediate)}},
        {RouterRaw, {std::bind(setIntValue, RouterRaw, _1), std::bind(getIntValue, RouterRaw)}},
        {ProbeRouter, {std::bind(setIntValue, ProbeRouter, _1), std::bind(getIntValue, ProbeRouter)}},
        {ReqCorrelate, {std::bind(setIntValue, ReqCorrelate, _1), std::bind(getIntValue, ReqCorrelate)}},
        {ReqRelaxed, {std::bind(setIntValue, ReqRelaxed, _1), std::bind(getIntValue, ReqRelaxed)}},
        {CurveServer, {std::bind(setIntValue, CurveServer, _1), std::bind(getIntValue, CurveServer)}},
        {CurvePublicKey, {std::bind(setByteArrayValue, CurvePublicKey, _1), std::bind(getByteArrayValue, CurvePublicKey, 41)}},
        {CurveSecretKey, {std::bind(setByteArrayValue, CurveSecretKey, _1), std::bind(getByteArrayValue, CurveSecretKey, 41)}},
        {CurveServerKey, {std::bind(setByteArrayValue, CurveServerKey, _1), std::bind(getByteArrayValue, CurveServerKey, 41)}},
        {ZapDomain, {std::bind(setByteArrayValue, ZapDomain, _1), std::bind(getByteArrayValue, ZapDomain, 255)}},
#endif
    });
}
