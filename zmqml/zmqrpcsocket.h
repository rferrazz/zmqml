#ifndef ZMQRPCSOCKET_H
#define ZMQRPCSOCKET_H

#include "zmqsocket.h"

class ZMQRPCSocket : public ZMQSocket
{
    Q_OBJECT
public:
    explicit ZMQRPCSocket(QObject *parent = 0);

signals:

public slots:
    void invokeRemoteMethod(const QString &method, const QVariantList &args);
    void invokeRemoteMethod(const QList<QByteArray> &prevParts, const QString &method, const QVariantList &args);

};

#endif // ZMQRPCSOCKET_H
