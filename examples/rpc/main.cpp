#include "zmqcontext.h"
#include "zmqsocket.h"
#include "zmqrpcsocket.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qqml.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<ZMQSocket>("zmq.Components", 1, 0, "ZMQSocket");
    qmlRegisterType<ZMQRPCSocket>("zmq.Components", 1, 0, "ZMQRPCSocket");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    return app.exec();
}
