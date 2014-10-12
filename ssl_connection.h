#ifndef SSL_CONNECTION_H
#define SSL_CONNECTION_H

#include <unordered_map>
#include <deque>

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
    asio::ssl::context &_context;
    ssl_socket *_socket;
    std::unordered_map<int, ssl_socket *> _connections;
    std::unordered_map<int, tcp::acceptor> _acceptors;
    std::unordered_map<int, tcp::resolver::query> _queries;
    std::deque<std::tuple<int, char *, std::size_t>> _queue;
};

#endif // SSL_CONNECTION_H
