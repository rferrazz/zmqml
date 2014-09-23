/*
 * ZmQML - QML binding for zeromq.
 *
 * Copyright (C) 2014 Riccardo Ferrazzo
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef REMOTEMETHOD_H
#define REMOTEMETHOD_H

#include <QObject>
#include <QByteArray>
#include <QDataStream>
#include <QVariant>

class RemoteMethod : public QObject
{
    Q_OBJECT
public:
    explicit RemoteMethod();

    QByteArray method;
    QVariantList args;
};

QDataStream &operator<<(QDataStream &s, const RemoteMethod &m);
QDataStream &operator>>(QDataStream &s, RemoteMethod &m);

#endif //REMOTEMETHOD_H
