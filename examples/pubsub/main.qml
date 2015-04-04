/*
 * ZmQML - QML binding for zeromq.
 *
 * Copyright (C) 2014 Riccardo Ferrazzo
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

import QtQuick 2.1
import QtQuick.Window 2.1

import zmq.Components 1.0

Window {
    visible: true
    width: 360
    height: 360

    ZMQSocket {
        id: sender
        type: ZMQSocket.Pub
        addresses: ["tcp://127.0.0.1:8000"]
    }

    ZMQSocket {
        id: receiver
        type:ZMQSocket.Sub
        addresses: ["tcp://127.0.0.1:8000"]
        subscriptions: [""]

        onMessageReceived: {
            console.log(BAT.stringify(message));
            text.text = BAT.stringify(message)[0];
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            sender.sendMessage(BAT.byteArrayfy("Hello world from zmq pubsub"));
        }
    }

    Text {
        id: text
        text: qsTr("Click to send the zmq message")
        anchors.centerIn: parent
    }

    Component.onCompleted: {
        sender.bindSocket();
        receiver.connectSocket();
    }
}
