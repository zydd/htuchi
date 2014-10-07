#include "basic_connection_layer.h"

basic_connection_layer::basic_connection_layer(asio::io_service &io_service)
    : _io_service(io_service),
      _socket(io_service)
{ }


void basic_connection_layer::_close()
{
    if (_socket.is_open()) {
        _socket.shutdown(tcp::socket::shutdown_send);
        _socket.close();
    }
}

basic_connection_layer::~basic_connection_layer()
{ }


void basic_connection_layer::receive()
{
    auto size_arr = new char[4];
    asio::async_read(_socket, asio::buffer(size_arr, 4),
                     [this, size_arr](const asio::error_code &error,
                                      const std::size_t &length)
    {
        if (error) {
            qDebug() << "receive()" << error.message().c_str();
            _socket.close();
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
                _socket.close();
                return;
            }
            qDebug() << "receive()" << length << "bytes";
            main_event_loop.post([this, message, size](){
                processIn({std::unique_ptr<char[]>(message), size});
            });

            receive();
        });
    });
}

void basic_connection_layer::send()
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    if (_queue.empty() || !_socket.is_open()) return;

    auto size = new char[4];
    for (int i = 0; i < 4; ++i)
        size[i] = (_queue.front().second >> (i * 8)) & 0xFF;

    asio::async_write(_socket, asio::buffer(size, 4),
                      [this, size](const asio::error_code &error,
                                   const std::size_t &/*length*/)
    {
        delete[] size;
        if (error) {
            qDebug() << error.message().c_str();
            _socket.close();
            return;
        }
        auto front = _queue.front();
        asio::async_write(_socket, asio::buffer(front.first, front.second),
                          [this](const asio::error_code &error,
                                 const std::size_t &length)
        {
            if (error) {
                qDebug() << error.message().c_str();
                _socket.close();
                return;
            }
            qDebug() << "send()" << length << "bytes";
            _mutex.lock();
            _queue.pop_front();
            _mutex.unlock();
            send();
        });
    });
}

void basic_connection_layer::processIn(const packet &data)
{
    if (_above) _above->processIn(data);
}

void basic_connection_layer::processOut(const packet &data)
{
    std::lock_guard<std::mutex> lock_guard(_mutex);

    _queue.emplace_back(data.seriallize(), data.size());

    if (_queue.size() == 1)
        _io_service.post(std::bind(&basic_connection_layer::send, this));
}
