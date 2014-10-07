#ifndef BYTEARRAYTOOLS_H
#define BYTEARRAYTOOLS_H

#include <QObject>

class ByteArrayTools : public QObject
{
    Q_OBJECT
public:
    explicit ByteArrayTools(QObject *parent = 0);

    Q_INVOKABLE QByteArray byteArrayfy(const QString &s) const;
    Q_INVOKABLE QList<QByteArray> byteArrayfy(const QList<QString> &l) const;

    Q_INVOKABLE QString stringify(const QByteArray &b) const;
    Q_INVOKABLE QList<QString> stringify(const QList<QByteArray> &l) const;

};

#endif // BYTEARRAYTOOLS_H
