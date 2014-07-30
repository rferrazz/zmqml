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
            text.text = message;
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            sender.sendMessage("Hello world from zmq pubsub");
        }
    }

    Text {
        id: text
        text: qsTr("Click to send the zmq message")
        anchors.centerIn: parent
    }
}
