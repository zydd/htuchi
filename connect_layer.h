#ifndef CONNECT_LAYER_H
#define CONNECT_LAYER_H

#include "basic_connection_layer.h"

class connect_layer : public basic_connection_layer
{
public:
    connect_layer(asio::io_service &io_service, const tcp::resolver::query &query);

};

#endif // CONNECT_LAYER_H
