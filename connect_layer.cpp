#include "connect_layer.h"

connect_layer::connect_layer(asio::io_service &io_service,
                             const tcp::resolver::query &query)
    : basic_connection_layer(io_service)
{
    _io_service.post([this, query]()
    {
        tcp::resolver resolver(_io_service);
        asio::async_connect(_socket, resolver.resolve(query),
                            [this](const asio::error_code &error,
                                const tcp::resolver::iterator &/*itr*/)
        {
            if (error) throw std::runtime_error("connection_layer::connect() " + error.message());
            receive();
            send();
        });
    });
}

