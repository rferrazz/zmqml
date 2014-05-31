#include "zmqcontext.h"
#include "zmqsocket.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qqml.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<ZMQSocket>("zmq.Components", 1, 0, "ZMQSocket");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    return app.exec();
}
