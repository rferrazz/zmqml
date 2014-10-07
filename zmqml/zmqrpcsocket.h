#ifndef ZMQRPCSOCKET_H
#define ZMQRPCSOCKET_H

#include "zmqsocket.h"

class ZMQRPCSocket : public ZMQSocket
{
    Q_OBJECT
public:
    explicit ZMQRPCSocket(QObject *parent = 0);

public slots:
    void invokeRemoteMethod(const QByteArray &method, const QVariantList &args);
    void invokeRemoteMethod(const QList<QByteArray> &prevParts, const QByteArray &method, const QVariantList &args);

};

#endif // ZMQRPCSOCKET_H
