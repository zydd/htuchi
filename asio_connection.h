#ifndef ASIO_CONNECTION_H
#define ASIO_CONNECTION_H

#include <mutex>
#include <deque>

#include <asio.hpp>

#include "packet.h"

using asio::ip::tcp;

class asio_connection
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
    void set_receive_callback(std::function<void(packet &&data)> callback);
    inline void close() { if(_socket.is_open()) _socket.close(); }
    inline bool is_open() { return _socket.is_open(); }

    virtual void send(packet &&data);

private:
    static const std::size_t _buffer_size = 65536 * 2;

    std::function<void()> disconnect_callback;
    std::function<void(packet &&data)> receive_callback;
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
