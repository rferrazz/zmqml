#ifndef ZMQSOCKET_H
#define ZMQSOCKET_H

#include "zmq.h"

#include <QObject>
#include <QUrl>
#include <QVariant>

class QSocketNotifier;
class ZMQContext;

class ZMQSocket : public QObject
{
    Q_OBJECT

    Q_ENUMS(ConnectionMethod SocketType)
    Q_PROPERTY(SocketType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QByteArray identity READ identity WRITE setIdentity NOTIFY identityChanged)
    Q_PROPERTY(ConnectionMethod method READ method WRITE setMethod NOTIFY methodChanged)
    Q_PROPERTY(QVariantList addresses READ addresses WRITE setAddresses NOTIFY addressesChanged)
    Q_PROPERTY(QByteArray subscription READ subscription WRITE setSubscription NOTIFY subscriptionChanged)
public:

    enum ConnectionMethod {
        Bind,
        Connect,
        Unset
    };

    enum SocketType {
        Pub = ZMQ_PUB,
        Sub = ZMQ_SUB,
        Push = ZMQ_PUSH,
        Pull = ZMQ_PULL,
        Req = ZMQ_REQ,
        Rep = ZMQ_REP,
        Dealer = ZMQ_DEALER,
        Router = ZMQ_ROUTER,
        Pair = ZMQ_PAIR,
        XPub = ZMQ_XPUB,
        XSub = ZMQ_XSUB,
        Null
    };

    explicit ZMQSocket(QObject *parent = 0);
    ~ZMQSocket();

    SocketType type() const;
    QByteArray identity() const;
    ConnectionMethod method() const;
    QVariantList addresses() const;
    QByteArray subscription() const;

    void setType(const SocketType type);
    void setIdentity(const QByteArray &id);
    void setMethod(const ConnectionMethod method);
    void setAddresses(const QVariantList &addresses);
    void setSubscription(const QByteArray &sub);


signals:
    void typeChanged();
    void identityChanged();
    void methodChanged();
    void addressesChanged();
    void subscriptionChanged();

    void messageReceived(const QList<QByteArray> &message);

public slots:
    void sendMessage(const QString &message);
    void sendMessage(const QList<QString> &message);

private:
    void setup();

private:
    SocketType _type;
    QByteArray _identity;
    ConnectionMethod _method;
    QVariantList _addr;
    QByteArray _subscription;

    void *socket;
    QSocketNotifier *notifier;

};

#endif // ZMQSOCKET_H
