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

    void testSockOptions_data();
    void testSockOptions();

};

Q_DECLARE_METATYPE(ZMQSocket::SockOption)

void TestSocket::testSocket_data()
{
    QTest::addColumn<ZMQSocket::SocketType>("first_type");
    QTest::addColumn< QVariantList >("first_addr");

    QTest::addColumn<ZMQSocket::SocketType>("second_type");
    QTest::addColumn< QVariantList >("second_addr");

    QTest::addColumn<QStringList> ("subscription");

    QTest::addColumn< QList<QByteArray> >("message");

    QTest::newRow("pub/sub short") << ZMQSocket::Pub << QVariantList({QUrl("tcp://127.0.0.1:5566")})
                                           << ZMQSocket::Sub << QVariantList({QUrl("tcp://127.0.0.1:5566")})
                                           << QStringList({""})
                                           << (QList<QByteArray>() << "Hello World");

    QTest::newRow("pub/sub multipart") << ZMQSocket::Pub << QVariantList({QUrl("tcp://127.0.0.1:6677")})
                                          << ZMQSocket::Sub << QVariantList({QUrl("tcp://127.0.0.1:6677")})
                                          << QStringList({"hello"})
                                          << (QList<QByteArray>() << "hello" << "world" << "!");

    QTest::newRow("req/rep short") << ZMQSocket::Req << QVariantList({QUrl("inproc://3")})
                                   << ZMQSocket::Rep << QVariantList({QUrl("inproc://3")})
                                   << QStringList({""})
                                   << (QList<QByteArray>() << "hello world");

    QTest::newRow("req/rep multipart") << ZMQSocket::Req << QVariantList({QUrl("inproc://3")})
                                   << ZMQSocket::Rep << QVariantList({QUrl("inproc://3")})
                                   << QStringList({""})
                                   << (QList<QByteArray>() << "hello" << "world");
}

void TestSocket::testSocket()
{
    QFETCH(ZMQSocket::SocketType, first_type);
    QFETCH(QVariantList, first_addr);

    QFETCH(ZMQSocket::SocketType, second_type);
    QFETCH(QVariantList, second_addr);
    QFETCH(QStringList, subscription);
    QFETCH(QList<QByteArray>, message);

    ZMQSocket first;
    ZMQSocket second;

    first.setType(first_type);
    first.setAddresses(first_addr);
    first.bindSocket();

    QCOMPARE(first.status(), ZMQSocket::Connected);

    second.setType(second_type);
    second.setAddresses(second_addr);
    second.setSubscriptions(subscription);
    second.connectSocket();

    QCOMPARE(second.status(), ZMQSocket::Connected);

    QSignalSpy spy(&second, SIGNAL(messageReceived(QList<QByteArray>)));

    QTest::qWait(100);

    first.sendMessage(message);

    QTest::qWait(100);

    QCOMPARE(spy.count(), 1);

    const QVariantList &args = spy.takeFirst();
    QCOMPARE(args.length(), 1);

    QCOMPARE(args.first().value< QList<QByteArray> >(), message);
}

void TestSocket::testSockOptions_data()
{
    QTest::addColumn<ZMQSocket::SockOption>("option");
    QTest::addColumn<QVariant>("value");

    QTest::newRow("sndhwm") << ZMQSocket::SndHwm << QVariant(3);
}

void TestSocket::testSockOptions()
{
    QFETCH(ZMQSocket::SockOption, option);
    QFETCH(QVariant, value);

    ZMQSocket s;
    s.setType(ZMQSocket::Dealer);
    s.setAddresses({"tcp://127.0.0.1:6677"});

    QVERIFY(s.setSockOption(option, value));
    QCOMPARE(s.getSockOption(option), value);
}

QTEST_MAIN(TestSocket)
#include "testsocket.moc"
