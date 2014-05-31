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
        method: ZMQSocket.Bind
    }

    ZMQSocket {
        id: receiver
        type:ZMQSocket.Sub
        addresses: ["tcp://127.0.0.1:8000"]
        method: ZMQSocket.Connect

        onMessageReceived: {
            console.log(message)
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            sender.sendMessage("ciao");
        }
    }

    Text {
        text: qsTr("Hello World")
        anchors.centerIn: parent
    }
}
