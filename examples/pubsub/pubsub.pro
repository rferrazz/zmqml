TEMPLATE = app

QT += qml quick
CONFIG += c++11

LIBS += -lzmq

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

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
