TEMPLATE = lib
TARGET = zmqml
QT += qml quick
CONFIG += qt plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = zmq.Components

include($$PWD/../zmq.pri)

# Input
SOURCES += \
    zmqml_plugin.cpp \
    zmqcontext.cpp \
    zmqsocket.cpp \
    remotemethod.cpp \
    zmqrpcsocket.cpp \
    bytearraytools.cpp

HEADERS += \
    zmq.h \
    zmqml_plugin.h \
    zmqcontext.h \
    zmqsocket.h \
    remotemethod.h \
    zmqrpcsocket.h \
    bytearraytools.h

OTHER_FILES = qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

