#include "zmqcontext.h"
#include "zmqsocket.h"
#include "bytearraytools.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qqml.h>

static QObject *byteArrayTools_factory(QQmlEngine *, QJSEngine *)
{
    return new ByteArrayTools;
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<ZMQSocket>("zmq.Components", 1, 0, "ZMQSocket");
    qmlRegisterSingletonType<ByteArrayTools>("zmq.Components", 1, 0, "BATools", byteArrayTools_factory);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    return app.exec();
}
