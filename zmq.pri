mac {
    # brew install zeromq
    INCLUDEPATH += /usr/local/Cellar/zeromq/4.0.5/include
    LIBS += -L/usr/local/lib -lzmq
} else:unix {
	LIBS += -lzmq
}
