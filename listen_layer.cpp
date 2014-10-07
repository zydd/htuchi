#include "listen_layer.h"

listen_layer::listen_layer(asio::io_service& io_service,
                           const tcp::endpoint& endpoint)
    : basic_connection_layer(io_service),
      _acceptor(io_service, endpoint)
{
    _io_service.post([this, endpoint]()
    {
        _acceptor.async_accept(_socket, [this](const asio::error_code &error)
        {
            if (error) throw std::runtime_error("connection_layer::accept() " + error.message());
            receive();
            send();
        });
    });
}
