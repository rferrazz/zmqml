mac {
    # brew install zeromq
    INCLUDEPATH += /usr/local/Cellar/zeromq/4.0.5/include
    LIBS += -L/usr/local/lib -lzmq
} else:unix {
	LIBS += -lzmq
}
else:win32-msvc*{
    ## READ THIS PLEASE
    ## Add your path and Lib here for windows Make sure that if you are using the ming you change the above part (win32-msvc2013)
    ## EXAMPLE:

    ## msvc2013
    LIBS += -L"C:/Program Files (x86)/ZeroMQ 4.0.4/lib/" -llibzmq-v120-mt-4_0_4
    INCLUDEPATH += "C:/Program Files (x86)/ZeroMQ 4.0.4/include"
}
