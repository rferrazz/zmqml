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

#include <zmq.h>

#include <QObject>
#include <QUrl>
#include <QVariant>
#include <QSet>
#include <QStringList>

#include <functional>

class QSocketNotifier;
class ZMQContext;

class ZMQSocket : public QObject
{
    Q_OBJECT

    Q_ENUMS(ConnectionStatus SocketType SockOption)
    Q_PROPERTY(ConnectionStatus status READ status NOTIFY connectionStatusChanged)
    Q_PROPERTY(SocketType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QByteArray identity READ identity WRITE setIdentity NOTIFY identityChanged)
    Q_PROPERTY(QVariantList addresses READ addresses WRITE setAddresses NOTIFY addressesChanged)
    Q_PROPERTY(QStringList subscriptions READ subscriptions WRITE setSubscriptions NOTIFY subscriptionsChanged)
public:

    enum ConnectionStatus {
        Invalid,
        Disconnected,
        Connected
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
        RecoveryIvl = ZMQ_RECOVERY_IVL,
        SndBuf = ZMQ_SNDBUF,
        RcvBuf = ZMQ_RCVBUF,
        Linger = ZMQ_LINGER,
        ReconnectIvl = ZMQ_RECONNECT_IVL,
        ReconnectIvlMax = ZMQ_RECONNECT_IVL_MAX,
        Backlog = ZMQ_BACKLOG,
        MulticastHops = ZMQ_MULTICAST_HOPS,
        SndTimeOut = ZMQ_SNDTIMEO,
        IPV4Only = ZMQ_IPV4ONLY,
        RouterMandatory = ZMQ_ROUTER_MANDATORY,
        XPubVerbose = ZMQ_XPUB_VERBOSE,
        TcpKeepalive = ZMQ_TCP_KEEPALIVE,
        TcpKeepaliveCnt = ZMQ_TCP_KEEPALIVE_CNT,
        TcpKeepaliveIdle = ZMQ_TCP_KEEPALIVE_IDLE,
        TcpKeepaliveIntvl = ZMQ_TCP_KEEPALIVE_INTVL,

#if ZMQ_VERSION_MAJOR > 3
        IPV6 = ZMQ_IPV6,
        Immediate = ZMQ_IMMEDIATE,
        RouterRaw = ZMQ_ROUTER_RAW,
        ProbeRouter = ZMQ_PROBE_ROUTER,
        ReqCorrelate = ZMQ_REQ_CORRELATE,
        ReqRelaxed = ZMQ_REQ_RELAXED,
        CurveServer = ZMQ_CURVE_SERVER,
        CurvePublicKey = ZMQ_CURVE_PUBLICKEY,
        CurveSecretKey = ZMQ_CURVE_SECRETKEY,
        CurveServerKey = ZMQ_CURVE_SERVERKEY,
        ZapDomain = ZMQ_ZAP_DOMAIN,
#endif
    };

    explicit ZMQSocket(QObject *parent = 0);
    ~ZMQSocket();

    ConnectionStatus status() const;
    SocketType type() const;
    QByteArray identity() const;
    QVariantList addresses() const;
    QStringList subscriptions() const;

    void setType(const SocketType type);
    void setIdentity(const QByteArray &id);
    void setAddresses(const QVariantList &addresses);
    void setSubscriptions(const QStringList &sub);

    Q_INVOKABLE bool setSockOption(SockOption option, const QVariant &value);
    Q_INVOKABLE QVariant getSockOption(SockOption option);

    Q_INVOKABLE bool connectSocket();
    Q_INVOKABLE bool bindSocket();

signals:
    void connectionStatusChanged();
    void typeChanged();
    void identityChanged();
    void methodChanged();
    void addressesChanged();
    void subscriptionsChanged();

    void messageReceived(const QList<QByteArray> &message);

public slots:
    void sendMessage(const QByteArray &message);
    void sendMessage(const QList<QByteArray> &message);

private:
    enum ConnectionMethod {
        Connect,
        Bind
    };

    struct Option {
        std::function<bool (const QVariant &)> setter;
        std::function<QVariant ()> getter;
    };

    void setup();
    bool setupConnection(ZMQSocket::ConnectionMethod method);
    void setupOptionsTable();

    QHash<SockOption, Option> options;

    ConnectionStatus _connection_status;
    ConnectionMethod _connection_method;
    SocketType _type;
    QByteArray _identity;
    QVariantList _addr;
    QSet<QString> _subscriptions;

    void *socket;
    QSocketNotifier *notifier;

};

#endif // ZMQSOCKET_H
