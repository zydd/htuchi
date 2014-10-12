#ifndef CONNECTION_LAYER_H
#define CONNECTION_LAYER_H

#include <deque>
#include <set>
#include <unordered_map>
#include <mutex>
#include <asio.hpp>

#include "abstract_layer.h"

#include "asio_acceptor.h"
#include "asio_connection.h"

using asio::ip::tcp;

class connection_layer : public abstract_layer
{
public:
    connection_layer();
    ~connection_layer();

    void add_acceptor(acceptor &&acc);
    void add_connection(connection &&conn);
    void receive(int id, const std::size_t &size, char *data);

    virtual void processIn(packet &&data);
    virtual void processOut(packet &&data);

protected:
    int _id = 0;

    std::mutex _mutex;
    std::unordered_map<int, connection> _connections;
    std::unordered_map<int, acceptor> _acceptors;

    void disconnected(int id);
};

#endif // CONNECTION_LAYER_H
