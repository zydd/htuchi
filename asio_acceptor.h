#ifndef ASIO_ACCEPTOR_H
#define ASIO_ACCEPTOR_H

#include <functional>

#include <asio.hpp>

#include "asio_connection.h"

using asio::ip::tcp;

class asio_acceptor
{
public:
    asio_acceptor(asio::io_service &io_service, const tcp::endpoint &endpoint);
    asio_acceptor(asio_acceptor &&o);
    ~asio_acceptor();

    void accept(std::function<void(asio_connection&&)> callback);

private:
    asio::io_service &_io_service;
    tcp::acceptor _acceptor;
    tcp::socket _socket;
};

#endif // ASIO_ACCEPTOR_H
