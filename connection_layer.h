#ifndef CONNECTION_LAYER_H
#define CONNECTION_LAYER_H

#include <deque>
#include <set>
#include <unordered_map>
#include <mutex>
#include <asio.hpp>

#include "abstract_layer.h"

using asio::ip::tcp;

class connection_layer : public abstract_layer
{
public:
    connection_layer(asio::io_service &io_service);
    ~connection_layer();

    virtual void processIn(packet &&data);
    virtual void processOut(packet &&data);

    inline void connect(const tcp::resolver::query &query)
    { _io_service.post([this, query](){ _connect(query); }); }

    inline void listen(const tcp::endpoint &endpoint)
    { _io_service.post([this, endpoint](){ _listen(endpoint); }); }

    inline void close()
    { _io_service.post([this](){ _close(); }); }

protected:
    asio::io_service &_io_service;
    std::mutex _mutex;
    std::unordered_map<int, tcp::socket> _connections;
    std::unordered_map<int, tcp::acceptor> _acceptors;
    std::unordered_map<int, tcp::resolver::query> _queries;
    std::deque<std::tuple<int, char *, std::size_t>> _queue;
    tcp::socket _socket;
    int _id = 0;

    void _connect(const tcp::resolver::query &query);
    void _listen(const tcp::endpoint &endpoint);
    void _close();
    void _close(int id);
    void accept(int id);
    void receive(int id);
    void send();
};

#endif // CONNECTION_LAYER_H
