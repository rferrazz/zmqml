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

    ZMQRPCSocket {
        id: sender
        type: ZMQSocket.Pub
        addresses: ["tcp://127.0.0.1:8000"]
        method: ZMQSocket.Bind
    }

    ZMQRPCSocket {
        id: receiver
        type:ZMQSocket.Sub
        addresses: ["tcp://127.0.0.1:8000"]
        method: ZMQSocket.Connect

        function writeHello(str, num, flo) {
            console.log("Method invoked");
            text.text = "Hello from %1, %2, %3".arg(str).arg(num).arg(flo);
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            sender.invokeRemoteMethod("writeHello", ["asd", 1, 3.2]);
        }
    }

    Text {
        id: text
        text: qsTr("Click to invoke the remote method")
        anchors.centerIn: parent
    }
}
