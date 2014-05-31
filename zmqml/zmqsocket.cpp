#include "zmqsocket.h"
#include "zmqcontext.h"

#include <QDebug>
#include <QSocketNotifier>
#include <QUuid>

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
    if(_method != Unset)
        return;

    _method = method;
    emit methodChanged();

    setup();
}

void ZMQSocket::setAddresses(const QVariantList &addresses)
{
    if (_addr.length() > 0)
        return;

    _addr = addresses;
    emit addressesChanged();

    setup();
}

void ZMQSocket::setSubscription(const QByteArray &sub)
{
    if (_subscription == sub)
        return;

    if (socket && _type == Sub){
        zmq_setsockopt(socket, ZMQ_UNSUBSCRIBE, (void *) _subscription.data(), _subscription.size());
        zmq_setsockopt(socket, ZMQ_SUBSCRIBE, (void *) sub.data(), sub.size());
    }

    _subscription = sub;
    emit subscriptionChanged();
}

void ZMQSocket::sendMessage(const QString &message)
{
    const QByteArray &msg = message.toLocal8Bit();
    qDebug() << "sending";
    const int res = zmq_send(socket, msg.constData(), msg.size(), 0);
    if (res == -1){
        qWarning() << "Error sending message";
        return;
    }
    qDebug() << "Message sent" << msg;
}

void ZMQSocket::sendMessage(const QList<QString> &message)
{
    QList<QString>::const_iterator i;
    for (i = message.constBegin(); i != message.constEnd(); ++i){
        int flags = (++i) == message.constEnd() ? 0 : ZMQ_SNDMORE;
        zmq_msg_t part;

        const int rc = zmq_msg_init_size(&part, (*i).size());
        if (rc != 0) {
            qDebug() << "Errore di inizializzazione messaggio";
            return;
        }

        memcpy(zmq_msg_data(&part), qPrintable(*i), (*i).size());

        zmq_sendmsg(socket, &part, flags);

        zmq_msg_close(&part);
    }
}

void ZMQSocket::setup()
{
    if (_type == Null || _method == Unset || _addr.length() == 0)
        return;

    socket = zmq_socket(ZMQContext::instance()->context, int(_type));

    if (!socket){
        qWarning() << "Socket creation error";
        return;
    }

    qDebug() << "Socket created";

    zmq_setsockopt(socket, ZMQ_IDENTITY, (void *) _identity.data(), _identity.size());

    qDebug() << "Identity setted";

    qint32 fd;
    size_t size = sizeof(fd);
    zmq_getsockopt(socket, ZMQ_FD, &fd, &size);

    notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);

    connect(notifier, &QSocketNotifier::activated,
        [=](){
            notifier->setEnabled(false);

            qint8 more = 1;
            QList<QByteArray> message;

            while(more == 1){
                int event;
                size_t ev_size = sizeof(event);
                zmq_getsockopt(socket, ZMQ_EVENTS, (void *) &event, &ev_size);

                if (!(event & ZMQ_POLLIN))
                    break;

                zmq_msg_t part;
                int rc = zmq_msg_init(&part);
                if (rc < 0) {
                    qDebug() << "Error initializing message";
                    return;
                }

                const int res = zmq_msg_recv(&part, socket, ZMQ_NOBLOCK);

                if (res < 0)
                    continue;

                message << QByteArray((char *) zmq_msg_data(&part), zmq_msg_size(&part));
                qDebug() << message;

                zmq_msg_close(&part);

                size_t size = sizeof(quint8);
                zmq_getsockopt(socket, ZMQ_RCVMORE, (void *)&more, &size);
            }
            emit messageReceived(message);
            notifier->setEnabled(true);
        }
    );

    if (_method == Connect) {
        int result;
        foreach(const QVariant &addr, _addr){
            result = zmq_connect(socket, qPrintable(addr.toUrl().toString()));
            qDebug() << "Connecting" << addr.toUrl();
            if (result == -1){
                qWarning() << "Connection error" << addr.toUrl().toString();
            }
        }
    } else {
        int result;
        foreach(const QVariant &addr, _addr){
            result = zmq_bind(socket, qPrintable(addr.toUrl().toString()));
            if (result == -1){
                qWarning() << "Bind error" << addr.toUrl().toString();
            }
        }
    }

    if (_type == Sub){
        zmq_setsockopt(socket, ZMQ_SUBSCRIBE, (void *) _subscription.data(), _subscription.size());
    }

    qDebug() << "Connected!";
}
