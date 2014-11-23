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

    void add_acceptor(asio_acceptor &&acc);
    void add_connection(asio_connection &&conn);
    void receive(int id, std::vector<byte> &&data);

    virtual void processIn(packet &&data);
    virtual void processOut(packet &&data);

protected:
    int _id = 0;

    std::mutex _mutex;
    std::unordered_map<int, asio_connection> _connections;
    std::unordered_map<int, asio_acceptor> _acceptors;

    void disconnected(int id);
};

#endif // CONNECTION_LAYER_H
