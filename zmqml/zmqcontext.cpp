#include "zmqcontext.h"

#include <QDebug>
#include <QMutex>

#include <zmq.hpp>

ZMQContext *ZMQContext::_instance = 0;

ZMQContext::ZMQContext(QObject *parent):
    QObject(parent)
{
   context = zmq_ctx_new();
   zmq_ctx_set(context, ZMQ_IO_THREADS, 4);
   zmq_ctx_set(context, ZMQ_MAX_SOCKETS, ZMQ_MAX_SOCKETS_DFLT);
}

ZMQContext::~ZMQContext()
{
    if (context)
        zmq_ctx_destroy(context);
}

ZMQContext *ZMQContext::instance()
{
    static QMutex mutex;

    mutex.lock();
    if (!_instance){
        _instance = new ZMQContext;
    }
    mutex.unlock();

    return _instance;
}

