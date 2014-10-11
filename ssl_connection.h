#ifndef SSL_CONNECTION_H
#define SSL_CONNECTION_H

// #include <unordered_map>
// #include <deque>

#include <asio.hpp>
#include <asio/ssl.hpp>

#include "abstract_layer.h"

using asio::ip::tcp;
using ssl_socket = asio::ssl::stream<tcp::socket>;

class ssl_connection : public abstract_layer
{
public:
    ssl_connection(asio::io_service &io_service, asio::ssl::context &context);

    virtual void processIn(packet &&data);
    virtual void processOut(packet &&data);

protected:
    asio::io_service &_io_service;
    ssl_socket _socket;
};

#endif // SSL_CONNECTION_H
