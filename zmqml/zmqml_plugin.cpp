/*
 * ZmQML - QML binding for zeromq.
 *
 * Copyright (C) 2014 Riccardo Ferrazzo
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "zmqml_plugin.h"
#include "zmqsocket.h"
#include "zmqrpcsocket.h"
#include "bytearraytools.h"

#include <qqml.h>
#include <QQmlEngine>

QML_DECLARE_TYPE(ByteArrayTools)

static QObject *byteArrayTools_factory(QQmlEngine *, QJSEngine *)
{
    return new ByteArrayTools;
}

void ZmqmlPlugin::registerTypes(const char *uri)
{
    // @uri zmq.Components
    qmlRegisterType<ZMQSocket>(uri, 1, 0, "ZMQSocket");
    qmlRegisterType<ZMQRPCSocket>(uri, 1, 0, "ZMQRPCSocket");
    qmlRegisterSingletonType<ByteArrayTools>(uri, 1, 0, "BATools", byteArrayTools_factory);
}


