/*
 * ZmQML - QML binding for zeromq.
 *
 * Copyright (C) 2014 Riccardo Ferrazzo
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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

    Q_ENUMS(ConnectionMethod SocketType SockOption)
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
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

    enum SockOption {
        SndHwm = ZMQ_SNDHWM,
        RcvHwm = ZMQ_RCVHWM,
        Rate = ZMQ_RATE,
        SndBuf = ZMQ_SNDBUF,
        RcvBuf = ZMQ_RCVBUF,
        Linger = ZMQ_LINGER,
        RecoveryIvl = ZMQ_RECOVERY_IVL,
        ReconnectIvl = ZMQ_RECONNECT_IVL,
        ReconnectIvlMax = ZMQ_RECONNECT_IVL_MAX,
        Backlog = ZMQ_BACKLOG,
        MulticastHops = ZMQ_MULTICAST_HOPS,
        IPV6 = ZMQ_IPV6,
        IPV4Only = ZMQ_IPV4ONLY,
        Immediate = ZMQ_IMMEDIATE,
        RouterMandatory = ZMQ_ROUTER_MANDATORY,
        RouterRaw = ZMQ_ROUTER_RAW,
        ProbeRouter = ZMQ_PROBE_ROUTER,
        XPubVerbose = ZMQ_XPUB_VERBOSE,
        ReqCorrelate = ZMQ_REQ_CORRELATE,
        ReqRelaxed = ZMQ_REQ_RELAXED,
        TcpKeepalive = ZMQ_TCP_KEEPALIVE,
        TcpKeepaliveCnt = ZMQ_TCP_KEEPALIVE_CNT,
        TcpKeepaliveIdle = ZMQ_TCP_KEEPALIVE_IDLE,
        TcpKeepaliveIntvl = ZMQ_TCP_KEEPALIVE_INTVL
    };

    explicit ZMQSocket(QObject *parent = 0);
    ~ZMQSocket();

    bool ready() const;
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

    Q_INVOKABLE bool setSockOption(SockOption option, int value);

signals:
    void readyChanged();
    void typeChanged();
    void identityChanged();
    void methodChanged();
    void addressesChanged();
    void subscriptionChanged();

    void messageReceived(const QStringList &message);

public slots:
    void sendMessage(const QString &message);
    void sendMessage(const QList<QString> &message);

protected:
    void sendMessage(const QList<QByteArray> &message);

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
