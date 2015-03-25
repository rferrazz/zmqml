QT += testlib

CONFIG += testcase c++11

include($$PWD/../zmq.pri)

INCLUDEPATH += $$PWD/../zmqml/

HEADERS += \
    $$PWD/../zmqml/zmqcontext.h \
    $$PWD/../zmqml/zmqsocket.h

SOURCES += \
    $$PWD/../zmqml/zmqcontext.cpp \
    $$PWD/../zmqml/zmqsocket.cpp \
    testsocket.cpp

