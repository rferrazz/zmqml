#include "zmqsocket.h"

#include <QTest>
#include <QSignalSpy>

Q_DECLARE_METATYPE(ZMQSocket::SocketType)

class TestSocket : public QObject
{
    Q_OBJECT
private slots:
    void testSocket_data();
    void testSocket();

};

void TestSocket::testSocket_data()
{
    QTest::addColumn<ZMQSocket::SocketType>("first_type");
    QTest::addColumn< QVariantList >("first_addr");

    QTest::addColumn<ZMQSocket::SocketType>("second_type");
    QTest::addColumn< QVariantList >("second_addr");

    QTest::addColumn<QByteArray> ("subscription");

    QTest::addColumn< QStringList >("message");

    QTest::newRow("pub/sub short") << ZMQSocket::Pub << (QVariantList() << QUrl("inproc://1"))
                                           << ZMQSocket::Sub << (QVariantList() << QUrl("inproc://1"))
                                           << QByteArray("")
                                           << (QStringList() << "Hello World");

    QTest::newRow("pub/sub multipart") << ZMQSocket::Pub << (QVariantList() << QUrl("inproc://2"))
                                          << ZMQSocket::Sub << (QVariantList() << QUrl("inproc://2"))
                                          << QByteArray("hello")
                                          << (QStringList() << "hello" << "world" << "!");

    QTest::newRow("req/rep short") << ZMQSocket::Req << (QVariantList() << QUrl("inproc://3"))
                                   << ZMQSocket::Rep << (QVariantList() << QUrl("inproc://3"))
                                   << QByteArray("")
                                   << (QStringList() << "hello world");

    QTest::newRow("req/rep multipart") << ZMQSocket::Req << (QVariantList() << QUrl("inproc://3"))
                                   << ZMQSocket::Rep << (QVariantList() << QUrl("inproc://3"))
                                   << QByteArray("")
                                   << (QStringList() << "hello" << "world");
}

void TestSocket::testSocket()
{
    QFETCH(ZMQSocket::SocketType, first_type);
    QFETCH(QVariantList, first_addr);

    QFETCH(ZMQSocket::SocketType, second_type);
    QFETCH(QVariantList, second_addr);
    QFETCH(QByteArray, subscription);
    QFETCH(QStringList, message);

    ZMQSocket first;
    ZMQSocket second;

    QSignalSpy firstReady(&first, SIGNAL(readyChanged()));
    QSignalSpy secondReady(&second, SIGNAL(readyChanged()));


    second.setType(second_type);
    second.setAddresses(second_addr);
    second.setMethod(ZMQSocket::Connect);
    second.setSubscription(subscription);

    QCOMPARE(secondReady.count(), 1);

    first.setType(first_type);
    first.setAddresses(first_addr);
    first.setMethod(ZMQSocket::Bind);

    QCOMPARE(firstReady.count(), 1);

    QSignalSpy spy(&second, SIGNAL(messageReceived(QStringList)));

    first.sendMessage(message);

    QTest::qWait(100);

    QCOMPARE(spy.count(), 1);

    const QVariantList &args = spy.takeFirst();
    QCOMPARE(args.length(), 1);

    QCOMPARE(args.first().value<QStringList>(), message);
}

QTEST_MAIN(TestSocket)
#include "testsocket.moc"
