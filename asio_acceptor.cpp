#include "asio_acceptor.h"

acceptor::acceptor(asio::io_service &io_service, const tcp::endpoint &endpoint)
    : _io_service(io_service),
      _acceptor(io_service, endpoint),
      _socket(io_service)
{ }

acceptor::acceptor(acceptor &&o)
    : _io_service(o._io_service),
      _acceptor(std::move(o._acceptor)),
      _socket(std::move(o._socket))
{ }

acceptor::~acceptor()
{ }

void acceptor::accept(std::function<void(connection&&)> callback)
{
    _acceptor.async_accept(_socket, [this, callback](const asio::error_code &error)
    {
        if (error) {
            qDebug() << "acceptor::accept()" << error.message().c_str();
            return;
        }
        qDebug() << "acceptor::accept()";
        callback({_io_service, std::move(_socket), tcp::resolver::query("","")});
        accept(callback);
    });
}
