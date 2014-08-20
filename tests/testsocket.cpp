#include "zmqsocket.h"

#include <QTest>
#include <QSignalSpy>

class TestSocket : public QObject
{
    Q_OBJECT
private slots:
    void testPubSub_data();
    void testPubSub();

};


void TestSocket::testPubSub_data()
{
    QTest::addColumn< QVariantList >("pub_addresses");
    QTest::addColumn< QVariantList >("sub_addresses");

    QTest::addColumn<QByteArray> ("subscription");

    QTest::addColumn< QStringList >("message");

    QTest::newRow("simple message") << (QVariantList() << QUrl("inproc://1"))
                                           << (QVariantList() << QUrl("inproc://1"))
                                           << QByteArray("")
                                           << (QStringList() << "Hello World");

    QTest::newRow("long message") << (QVariantList() << QUrl("inproc://2"))
                                      << (QVariantList() << QUrl("inproc://2"))
                                      << QByteArray("hello")
                                      << (QStringList() << "hello" << "world" << "!");
}

void TestSocket::testPubSub()
{
    QFETCH(QVariantList, pub_addresses);
    QFETCH(QVariantList, sub_addresses);
    QFETCH(QByteArray, subscription);
    QFETCH(QStringList, message);

    ZMQSocket sub;
    ZMQSocket pub;

    QSignalSpy subReady(&sub, SIGNAL(readyChanged()));
    QSignalSpy pubReady(&pub, SIGNAL(readyChanged()));


    sub.setType(ZMQSocket::Sub);
    sub.setAddresses(sub_addresses);
    sub.setMethod(ZMQSocket::Connect);
    sub.setSubscription(subscription);

    QCOMPARE(subReady.count(), 1);

    pub.setType(ZMQSocket::Pub);
    pub.setAddresses(pub_addresses);
    pub.setMethod(ZMQSocket::Bind);

    QCOMPARE(pubReady.count(), 1);

    QSignalSpy spy(&sub, SIGNAL(messageReceived(QStringList)));

    pub.sendMessage(message);

    QTest::qWait(100);

    QCOMPARE(spy.count(), 1);

    const QVariantList &args = spy.takeFirst();
    QCOMPARE(args.length(), 1);

    QCOMPARE(args.first().value<QStringList>(), message);
}

QTEST_MAIN(TestSocket)
#include "testsocket.moc"
