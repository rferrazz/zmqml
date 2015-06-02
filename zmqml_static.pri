
INCLUDEPATH += $$PWD/zmqml

mac {
    # brew install zeromq
    INCLUDEPATH += /usr/local/Cellar/zeromq/4.0.5_2/include
    LIBS += -L/usr/local/lib -lzmq
} else:unix {
        LIBS += -lzmq
}

SOURCES += \
    $$PWD/zmqml/zmqcontext.cpp \
    $$PWD/zmqml/zmqsocket.cpp \
    $$PWD/zmqml/remotemethod.cpp \
    $$PWD/zmqml/zmqrpcsocket.cpp \
    $$PWD/zmqml/bytearraytools.cpp \
    $$PWD/zmqml/zmqml_plugin.cpp \

HEADERS += \
    $$PWD/zmqml/zmqcontext.h \
    $$PWD/zmqml/zmqsocket.h \
    $$PWD/zmqml/remotemethod.h \
    $$PWD/zmqml/zmqrpcsocket.h \
    $$PWD/zmqml/bytearraytools.h \
    $$PWD/zmqml/zmqml_plugin.h

