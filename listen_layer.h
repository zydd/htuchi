#ifndef LISTEN_LAYER_H
#define LISTEN_LAYER_H

#include "basic_connection_layer.h"

class listen_layer : public basic_connection_layer
{
public:
    listen_layer(asio::io_service &io_service, const tcp::endpoint &endpoint);

private:
    tcp::acceptor _acceptor;
};

#endif // LISTEN_LAYER_H
