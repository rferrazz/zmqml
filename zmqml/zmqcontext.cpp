/*
 * ZmQML - QML binding for zeromq.
 *
 * Copyright (C) 2014 Riccardo Ferrazzo
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "zmqcontext.h"

#include <QDebug>
#include <QMutex>

#include <zmq.h>

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
    if (!_instance) {
        _instance = new ZMQContext;
    }
    mutex.unlock();

    return _instance;
}

