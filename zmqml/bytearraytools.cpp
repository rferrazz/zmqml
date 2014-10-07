#include "bytearraytools.h"

ByteArrayTools::ByteArrayTools(QObject *parent) :
    QObject(parent)
{
}

QByteArray ByteArrayTools::byteArrayfy(const QString &s) const
{
    return s.toUtf8();
}

QList<QByteArray> ByteArrayTools::byteArrayfy(const QList<QString> &l) const
{
    QList<QByteArray> ret;
    foreach (const QString &s, l){
        ret << s.toUtf8();
    }

    return ret;
}

QString ByteArrayTools::stringify(const QByteArray &b) const
{
    return QString(b);
}

QList<QString> ByteArrayTools::stringify(const QList<QByteArray> &l) const
{
    QList<QString> res;
    foreach (const QByteArray &b, l) {
        res << QString(b);
    }

    return res;
}
