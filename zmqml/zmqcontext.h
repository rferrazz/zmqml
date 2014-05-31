#ifndef ZMQCONTEXT_H
#define ZMQCONTEXT_H

#include "zmqsocket.h"

#include <QObject>
#include <QList>

class ZMQContext : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ZMQContext)

public:
    ~ZMQContext();
    static ZMQContext *instance();

    void *context;

private:
    ZMQContext(QObject *parent = 0);
    static ZMQContext *_instance;
};

#endif // ZMQCONTEXT_H

