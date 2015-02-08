#include "asio_acceptor.h"

asio_acceptor::asio_acceptor(asio::io_service &io_service, const tcp::endpoint &endpoint)
    : _io_service(io_service),
      _acceptor(io_service, endpoint),
      _socket(io_service)
{ }

asio_acceptor::asio_acceptor(asio_acceptor &&o)
    : _io_service(o._io_service),
      _acceptor(std::move(o._acceptor)),
      _socket(std::move(o._socket))
{ }

asio_acceptor::~asio_acceptor()
{ }

void asio_acceptor::accept(std::function<void(asio_connection&&)> callback)
{
    _acceptor.async_accept(_socket, [this, callback](const asio::error_code &error)
    {
        if (error) {
//             qDebug() << "acceptor::accept()" << error.message().c_str();
            return;
        }
        callback({_io_service, std::move(_socket), tcp::resolver::query("","")});
        accept(callback);
    });
}
