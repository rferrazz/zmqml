TEMPLATE = app

QT += qml quick
CONFIG += c++11

include($$PWD/../../zmq.pri)

INCLUDEPATH += $$PWD/../../zmqml/

SOURCES += \
    $$PWD/../../zmqml/zmqcontext.cpp \
    $$PWD/../../zmqml/zmqsocket.cpp \
    $$PWD/../../zmqml/bytearraytools.cpp \
    main.cpp

HEADERS += \
    $$PWD/../../zmqml/zmqcontext.h \
    $$PWD/../../zmqml/zmqsocket.h \
    $$PWD/../../zmqml/bytearraytools.h \

RESOURCES += qml.qrc
