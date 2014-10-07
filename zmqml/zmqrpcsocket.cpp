#include "zmqrpcsocket.h"

#include "remotemethod.h"

#include <QDataStream>
#include <QStringList>
#include <QByteArray>
#include <QDebug>
#include <QMetaMethod>
#include <QString>

ZMQRPCSocket::ZMQRPCSocket(QObject *parent) :
    ZMQSocket(parent)
{
    connect(this, &ZMQRPCSocket::messageReceived,
            this, [this](const QList<QByteArray> &message)
    {
        RemoteMethod method;

        QByteArray last = message.last();
        QDataStream out(&last, QIODevice::ReadOnly);
        out.setVersion(QDataStream::Qt_5_0);

        out >> method;

        // Try to invoke matching javascript functions inside this object

        QList<QGenericArgument> args;
        QList<QVariant>::const_iterator i;
        for(i = method.args.constBegin(); i != method.args.constEnd(); ++i){
            args << Q_ARG(QVariant, (*i));
        }

        const QMetaObject *mo = this->metaObject();

        QString signature = QString("%1()").arg(QString(method.method));
        for (int i = 0; i < method.args.count(); ++i){
            signature.insert(signature.length() - 1, "QVariant,");
        }
        signature.remove(signature.length() -2, 1);

        bool result = false;
        int index = mo->indexOfMethod(qPrintable(signature));
        if (index != -1) {
            QMetaMethod method = mo->method(index);
            result = method.invoke(this,
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
        }

        if (result)
            return;

        //TODO: search for a pure C++ method with a matching signature
    });
}

void ZMQRPCSocket::invokeRemoteMethod(const QByteArray &method, const QVariantList &args)
{
    invokeRemoteMethod(QList<QByteArray>(), method, args);
}

void ZMQRPCSocket::invokeRemoteMethod(const QList<QByteArray> &prevParts, const QByteArray &method, const QVariantList &args)
{
    RemoteMethod m;
    m.method = method;
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
