#ifndef CONNECTION_LAYER_H
#define CONNECTION_LAYER_H

#include <deque>
#include <set>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <asio.hpp>

#include "abstract_layer.h"

#include "asio_acceptor.h"
#include "asio_connection.h"

using asio::ip::tcp;

class connection_layer : public abstract_layer
{
public:
    enum Status {
        Offline = 0,
        Online  = 1 << 0,
    };

    connection_layer();
    ~connection_layer();
    void add_acceptor(asio_acceptor &&acc);
    void add_connection(asio_connection &&conn);
    void receive(int id, packet &&data);

    void set_connection_change(std::function<void(int id, int status)> callback)
    { _connection_change = callback; }

private:
    int _gen_id = 0;

    std::mutex _mutex;
    std::unordered_map<int, asio_connection> _connections;
    std::unordered_map<int, asio_acceptor> _acceptors;
    std::function<void(int id, int status)> _connection_change;

    void disconnected(int id);
    virtual void processDown(packet &&data);
};

#endif // CONNECTION_LAYER_H
