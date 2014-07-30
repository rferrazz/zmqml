/*
 * ZmQML - QML binding for zeromq.
 *
 * Copyright (C) 2014 Riccardo Ferrazzo
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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

