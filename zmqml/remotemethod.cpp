/*
 * ZmQML - QML binding for zeromq.
 *
 * Copyright (C) 2014 Riccardo Ferrazzo
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "remotemethod.h"
#include <QDebug>


RemoteMethod::RemoteMethod() :
    QObject()
{
}


QDataStream &operator<<(QDataStream &s, const RemoteMethod &m)
{
    s << m.method;
    s << m.args;
    return s;
}


QDataStream &operator>>(QDataStream &s, RemoteMethod &m)
{
    s >> m.method;
    s >> m.args;
    return s;
}
