TEMPLATE = app

QT += qml quick
CONFIG += c++11

include($$PWD/../../zmq.pri)

INCLUDEPATH += $$PWD/../../zmqml/

SOURCES += \
    $$PWD/../../zmqml/zmqcontext.cpp \
    $$PWD/../../zmqml/zmqsocket.cpp \
    $$PWD/../../zmqml/zmqrpcsocket.cpp \
    $$PWD/../../zmqml/remotemethod.cpp \
    main.cpp

HEADERS += \
    $$PWD/../../zmqml/zmqcontext.h \
    $$PWD/../../zmqml/zmqsocket.h \
    $$PWD/../../zmqml/zmqrpcsocket.h \
    $$PWD/../../zmqml/remotemethod.h \

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
