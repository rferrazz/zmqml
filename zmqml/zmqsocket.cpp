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

inline void zmqError(const QString &message) {
    qWarning() << message << zmq_strerror(zmq_errno());
}

ZMQSocket::ZMQSocket(QObject *parent) :
    QObject(parent),
    _type(Null),
    _identity(QUuid::createUuid().toByteArray()),
    _method(Unset),
    socket(0)
{}

ZMQSocket::~ZMQSocket()
{
    if (socket)
        zmq_close(socket);
}

bool ZMQSocket::ready() const
{
    return bool(socket);
}

ZMQSocket::SocketType ZMQSocket::type() const
{
    return _type;
}

QByteArray ZMQSocket::identity() const
{
    return _identity;
}

ZMQSocket::ConnectionMethod ZMQSocket::method() const
{
    return _method;
}

QVariantList ZMQSocket::addresses() const
{
    return _addr;
}

QByteArray ZMQSocket::subscription() const
{
    return _subscription;
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
        zmq_setsockopt(socket, ZMQ_IDENTITY, (void *) id.data(), id.size());
    }

    _identity == id;
    emit identityChanged();
}

void ZMQSocket::setMethod(const ZMQSocket::ConnectionMethod method)
{
    if (_method != Unset)
        return;

    _method = method;
    emit methodChanged();

    setup();
}

void ZMQSocket::setAddresses(const QVariantList &addresses)
{
    if (_addr.length() > 0 && socket) {
        int (*dfun)(void *, const char*) = _method == Connect ? zmq_disconnect : zmq_unbind;

        foreach (const QVariant &a, _addr) {
            int rc = dfun(socket, qPrintable(a.toUrl().toString()));

            if (rc < 0)
                zmqError("Disconnection error:");
        }


        int (*cfun)(void *, const char*) = _method == Connect ? zmq_connect : zmq_bind;
        int result;

        foreach(const QVariant &addr, addresses) {
            result = cfun(socket, qPrintable(addr.toUrl().toString()));

            if (result == -1)
                zmqError("Connection error:");
        }

    }

    _addr = addresses;
    emit addressesChanged();

    if (!socket)
        setup();
}

void ZMQSocket::setSubscription(const QByteArray &sub)
{
    if (_subscription == sub)
        return;

    if (socket && _type == Sub) {
        zmq_setsockopt(socket, ZMQ_UNSUBSCRIBE, (void *) _subscription.data(), _subscription.size());
        zmq_setsockopt(socket, ZMQ_SUBSCRIBE, (void *) sub.data(), sub.size());
    }

    _subscription = sub;
    emit subscriptionChanged();
}

bool ZMQSocket::setSockOption(ZMQSocket::SockOption option, int value)
{
    if (!socket) {
        qWarning() << "Error, socket not ready";
        return false;
    }

    const int rc = zmq_setsockopt(socket, int(option), (void*) &value, size_t(sizeof(int)));

    return rc >= 0;
}

void ZMQSocket::sendMessage(const QString &message)
{
    const QByteArray &msg = message.toLocal8Bit();

    const int res = zmq_send(socket, msg.constData(), msg.size(), 0);

    if (res == -1) {
        zmqError("Error sending message:");
        return;
    }
}

void ZMQSocket::sendMessage(const QList<QString> &message)
{
    QList<QString>::const_iterator i;
    for (i = message.constBegin(); i != message.constEnd(); ++i) {
        int flags = (i+1) == message.constEnd() ? 0 : ZMQ_SNDMORE;
        const QByteArray &msg = (*i).toLocal8Bit();

        const int res = zmq_send(socket, msg.constData(), msg.size(), flags);

        if (res == -1) {
            zmqError("Error sending message:");
            return;
        }
    }
}

void ZMQSocket::setup()
{
    if (_type == Null || _method == Unset || _addr.length() == 0)
        return;

    socket = zmq_socket(ZMQContext::instance()->context, int(_type));

    if (!socket) {
        zmqError("Socket creation error:");
        return;
    }

    emit readyChanged();

    zmq_setsockopt(socket, ZMQ_IDENTITY, (void *) _identity.data(), _identity.size());

    qint32 fd;
    size_t size = sizeof(fd);
    zmq_getsockopt(socket, ZMQ_FD, &fd, &size);

    notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);

    connect(notifier, &QSocketNotifier::activated,
        [=]() {
            notifier->setEnabled(false);

            qint8 more = 1;
            QStringList message;

            while(more == 1){
                int event;
                size_t ev_size = sizeof(event);
                zmq_getsockopt(socket, ZMQ_EVENTS, (void *) &event, &ev_size);

                if (!(event & ZMQ_POLLIN))
                    break;

                zmq_msg_t part;
                int rc = zmq_msg_init(&part);
                if (rc < 0) {
                    zmqError("Error initializing message:");
                    break;
                }

                const int res = zmq_msg_recv(&part, socket, ZMQ_NOBLOCK);

                if (res < 0)
                    continue;

                message << QString::fromLocal8Bit((char *) zmq_msg_data(&part), zmq_msg_size(&part));

                zmq_msg_close(&part);

                size_t size = sizeof(quint8);
                zmq_getsockopt(socket, ZMQ_RCVMORE, (void *)&more, &size);
            }

            if (message.length() > 0)
                emit messageReceived(message);

            notifier->setEnabled(true);
        }
    );

    int (*fun)(void *, const char*) = _method == Connect ? zmq_connect : zmq_bind;
    int result;

    foreach(const QVariant &addr, _addr) {
        result = fun(socket, qPrintable(addr.toUrl().toString()));

        if (result == -1)
            zmqError("Connection error:");
    }

    if (_type == Sub) {
        zmq_setsockopt(socket, ZMQ_SUBSCRIBE, (void *) _subscription.data(), _subscription.size());
    }
}
