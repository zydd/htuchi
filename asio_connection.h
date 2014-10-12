#ifndef ASIO_CONNECTION_H
#define ASIO_CONNECTION_H

#include <mutex>
#include <deque>

#include <QDebug>
#include <asio.hpp>

using asio::ip::tcp;

class connection
{
public:
    connection(asio::io_service &io_service,
               tcp::socket &&socket,
               const tcp::resolver::query &query);
    connection(asio::io_service &io_service, const tcp::resolver::query &query);
    connection(connection &&o);
    ~connection();

    void connect(std::function<void()> callback);
    void receive(std::function<void(const std::size_t &size, char *data)> callback);
    void send(const std::size_t &size, std::unique_ptr<char[]> &&data);
    void set_disconnect_callback(std::function<void()> callback);
    inline void close() { if(_socket.is_open()) _socket.close(); }
    inline bool is_open() { return _socket.is_open(); }

private:
    std::function<void()> disconnect_callback;
    asio::io_service &_io_service;
    tcp::socket _socket;
    std::mutex _mutex;
    tcp::resolver::query _query;
    std::deque<std::pair<std::size_t, std::unique_ptr<char[]>>> _queue;

    void write_next();
};

#endif // ASIO_CONNECTION_H
