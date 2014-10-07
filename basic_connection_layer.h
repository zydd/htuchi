#ifndef SERVER_CONNECT_H
#define SERVER_CONNECT_H

#include <deque>
#include <mutex>
#include <asio.hpp>

#include "abstract_layer.h"

using asio::ip::tcp;

class basic_connection_layer : public abstract_layer
{
public:
    basic_connection_layer(asio::io_service &io_service);
    ~basic_connection_layer();

    virtual void processIn(const packet &data);
    virtual void processOut(const packet &data);

    inline void close()
    { _io_service.post([this](){ _close(); }); }

protected:
    asio::io_service &_io_service;
    tcp::socket _socket;
    std::deque<std::pair<char *, std::size_t>> _queue;
    std::mutex _mutex;

    void _close();
    void receive();
    void send();
};

#endif // SERVER_CONNECT_H
