#ifndef ASIO_CONNECTION_H
#define ASIO_CONNECTION_H

#include <mutex>
#include <deque>

#include <QDebug>
#include <asio.hpp>

#include "abstract_layer.h"
#include "packet.h"

using asio::ip::tcp;

class asio_connection : public abstract_layer
{
public:
    asio_connection(asio::io_service &io_service,
               tcp::socket &&socket,
               const tcp::resolver::query &query);
    asio_connection(asio::io_service &io_service, const tcp::resolver::query &query);
    asio_connection(asio_connection &&o);
    ~asio_connection();

    void connect(std::function<void()> callback);
    void receive();
    void write_next();
    void set_disconnect_callback(std::function<void()> callback);
    inline void close() { if(_socket.is_open()) _socket.close(); }
    inline bool is_open() { return _socket.is_open(); }

    virtual void processIn(packet &&data);
    virtual void processOut(packet &&data);

private:
    static const std::size_t _buffer_size = 65536 * 2;

    std::function<void()> disconnect_callback;
    asio::io_service &_io_service;
    tcp::socket _socket;
    std::mutex _mutex;
    tcp::resolver::query _query;
    std::deque<packet> _queue;
    std::vector<char> _buffer;
    unsigned char _size_buffer_in[4];
    unsigned char _size_buffer_out[4];
    bool _writing_queue = false;
};

#endif // ASIO_CONNECTION_H
