TEMPLATE = subdirs
SUBDIRS = zmqml testsocket

testsocket.file = tests/testsocket.pro

examples {
    SUBDIRS += \
        examples/pubsub \
        examples/rpc
}
