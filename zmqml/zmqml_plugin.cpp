#include "zmqml_plugin.h"
#include "zmqsocket.h"

#include <qqml.h>

void ZmqmlPlugin::registerTypes(const char *uri)
{
    // @uri zmq.Components
    qmlRegisterType<ZMQSocket>(uri, 1, 0, "ZMQSocket");
}


