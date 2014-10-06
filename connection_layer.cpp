#include <vector>

#include "connection_layer.h"

connection_layer::connection_layer(asio::io_service &io_service)
    : _io_service(io_service),
      _socket(io_service)
{ }

void connection_layer::_connect(const tcp::resolver::query &query)
{
    tcp::resolver resolver(_io_service);

    asio::async_connect(_socket, resolver.resolve(query),
                        [this](const asio::error_code &error,
                               const tcp::resolver::iterator &/*itr*/)
    {
        if (error) throw std::runtime_error("connection_layer::connect() " + error.message());
        receive();
        send();
    });
}

void connection_layer::_accept(const tcp::endpoint &endpoint)
{
    _acceptor = new tcp::acceptor(_io_service, endpoint);
    _acceptor->async_accept(_socket, [this](const asio::error_code &error)
    {
        if (error) throw std::runtime_error("connection_layer::accept() " + error.message());
        receive();
        send();
    });
}

void connection_layer::_close()
{
    if (_socket.is_open()) {
        _socket.shutdown(tcp::socket::shutdown_send);
        _socket.close();
    }
}

connection_layer::~connection_layer()
{
    delete _acceptor;
}

// #include <thread>
// #include <chrono>

void connection_layer::receive()
{
    auto size_arr = new char[4];
    asio::async_read(_socket, asio::buffer(size_arr, 4),
                     [this, size_arr](const asio::error_code &error,
                                      const std::size_t &length)
    {
        if (error) {
            qDebug() << "receive()" << error.message().c_str();
            return;
        }

        std::size_t size = size_arr[0];
        size += size_arr[1] << 8;
        size += size_arr[2] << 16;
        size += size_arr[3] << 24;
        delete[] size_arr;

        auto message = new char[size];
        asio::async_read(_socket, asio::buffer(message, size),
                         [this, message, size](const asio::error_code &error,
                                               const std::size_t &length)
        {
            if (error) {
                qDebug() << "receive()" << error.message().c_str();
                return;
            }
            qDebug() << "receive()" << (length + 4) << "bytes";
            packet msg(message, size);
            main_event_loop.post([this, msg](){
                processIn(msg);
            });
            delete[] message;

            receive();
        });
    });
}

void connection_layer::send()
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    if (_queue.empty() || !_socket.is_open()) return;
    asio::async_write(_socket,asio::buffer(_queue.front()),
                      [this](const asio::error_code &error, const std::size_t &length)
    {
        if (error) {
            qDebug() << error.message().c_str();
            return;
        }
        qDebug() << "send()" << length << "bytes";
        _mutex.lock();
        _queue.pop_front();
        _mutex.unlock();
        send();
    });
}

void connection_layer::processIn(const packet &data)
{
    if (_above) _above->processIn(data);
}

void connection_layer::processOut(const packet &data)
{
    std::lock_guard<std::mutex> lock_guard(_mutex);

    char size[4];
    for (int i = 0; i < 4; ++i)
        size[i] = (data.size() >> (i * 8)) & 0xFF;

    _queue.emplace_back(std::vector<char>(data.size() + 4));

    std::copy(size, size + 4, _queue.back().begin());
    std::copy(data.begin(), data.end(), _queue.back().begin() + 4);

    if (_queue.size() == 1)
        _io_service.post(std::bind(&connection_layer::send, this));
}
