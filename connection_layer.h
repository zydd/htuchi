#ifndef SERVER_CONNECT_H
#define SERVER_CONNECT_H

#include <deque>
#include <mutex>
#include <asio.hpp>

#include "abstract_layer.h"

using asio::ip::tcp;

class connection_layer : public abstract_layer
{
public:
    connection_layer(asio::io_service &io_service);
    ~connection_layer();

    virtual void processIn(const packet &data);
    virtual void processOut(const packet &data);

    inline void connect(const tcp::resolver::query &query)
    { _io_service.post([this, query](){ _connect(query); }); }

    inline void accept(const tcp::endpoint &endpoint)
    { _io_service.post([this, endpoint](){ _accept(endpoint); }); }

    inline void close()
    { _io_service.post([this](){ _close(); }); }

private:
    asio::io_service &_io_service;
    tcp::socket _socket;
    tcp::acceptor *_acceptor = nullptr;
    std::deque<std::vector<char> > _queue;
    std::mutex _mutex;

    void _connect(const tcp::resolver::query &query);
    void _accept(const tcp::endpoint &endpoint);
    void _close();
    void receive();
    void send();
};

#endif // SERVER_CONNECT_H
