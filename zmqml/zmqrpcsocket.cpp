#include "zmqrpcsocket.h"

#include "remotemethod.h"

#include <QDataStream>
#include <QStringList>
#include <QByteArray>
#include <QDebug>

ZMQRPCSocket::ZMQRPCSocket(QObject *parent) :
    ZMQSocket(parent)
{
    connect(this, &ZMQRPCSocket::messageReceived,
            this, [this](const QStringList &message)
    {
        RemoteMethod method;

        QByteArray last = message.last().toLatin1();
        QDataStream out(&last, QIODevice::ReadOnly);
        out.setVersion(QDataStream::Qt_5_0);

        out >> method;

        QList<QGenericArgument> args;
        QList<QVariant>::const_iterator i;
        for (i = method.args.constBegin(); i != method.args.constEnd(); ++i){
            args << Q_ARG(QVariant, (*i).data());
        }

        static bool exists = QMetaObject::invokeMethod(this,
                                                       qPrintable(method.method),
                                                       args.value(0, QGenericArgument()),
                                                       args.value(1, QGenericArgument()),
                                                       args.value(2, QGenericArgument()),
                                                       args.value(3, QGenericArgument()),
                                                       args.value(4, QGenericArgument()),
                                                       args.value(5, QGenericArgument()),
                                                       args.value(6, QGenericArgument()),
                                                       args.value(7, QGenericArgument()),
                                                       args.value(8, QGenericArgument()),
                                                       args.value(9, QGenericArgument()));
        if (!exists)
            qWarning() << "Remote method does not exists:" << method.method;


    });
}

void ZMQRPCSocket::invokeRemoteMethod(const QString &method, const QVariantList &args)
{
    invokeRemoteMethod(QList<QByteArray>(), method, args);
}

void ZMQRPCSocket::invokeRemoteMethod(const QList<QByteArray> &prevParts, const QString &method, const QVariantList &args)
{
    RemoteMethod m;
    m.method = method.toLatin1();
    m.args = args;

    QByteArray content;
    QList<QByteArray> message;
    message << prevParts;

    QDataStream stream(&content, QIODevice::WriteOnly);

    stream.setVersion(QDataStream::Qt_5_0);
    stream << m;

    message << content;

    sendMessage(message);
}
