#ifndef ASIO_ACCEPTOR_H
#define ASIO_ACCEPTOR_H

#include <functional>

#include <asio.hpp>

#include "asio_connection.h"

using asio::ip::tcp;

class acceptor
{
public:
    acceptor(asio::io_service &io_service, const tcp::endpoint &endpoint);
    acceptor(acceptor &&o);
    ~acceptor();

    void accept(std::function<void(connection&&)> callback);

private:
    asio::io_service &_io_service;
    tcp::acceptor _acceptor;
    tcp::socket _socket;
};

#endif // ASIO_ACCEPTOR_H
