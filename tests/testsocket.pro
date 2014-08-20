QT += testlib

CONFIG += testcase c++11

LIBS += -lzmq

INCLUDEPATH += $$PWD/../zmqml/

HEADERS += \
    $$PWD/../zmqml/zmqcontext.h \
    $$PWD/../zmqml/zmqsocket.h

SOURCES += \
    $$PWD/../zmqml/zmqcontext.cpp \
    $$PWD/../zmqml/zmqsocket.cpp \
    testsocket.cpp

