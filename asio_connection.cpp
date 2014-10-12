#include "asio_connection.h"

connection::connection(asio::io_service &io_service,
                       tcp::socket &&socket,
                       const tcp::resolver::query &query)
    : _io_service(io_service),
      _socket(std::move(socket)),
      _query(query)
{ }

connection::connection(asio::io_service &io_service, const tcp::resolver::query &query)
    : _io_service(io_service),
      _socket(io_service),
      _query(query)
{ }

connection::connection(connection &&o)
    : _io_service(o._io_service),
      _socket(std::move(o._socket)),
      _query(o._query)
{

}

connection::~connection()
{
    if (_socket.is_open()) {
        _socket.shutdown(tcp::socket::shutdown_send);
        _socket.close();
    }
}

void connection::connect(std::function<void()> callback)
{
    tcp::resolver resolver(_io_service);
    asio::async_connect(_socket, resolver.resolve(_query),
                        [this, callback](const asio::error_code &error,
                                         const tcp::resolver::iterator &/*itr*/)
                        {
                            if (error) {
                                qDebug() << "connection::connect()" << error.message().c_str();
                                return;
                            }
                            callback();
                            write_next();
                        });
}

void connection::receive(std::function<void(const std::size_t &size, char *data)> callback)
{
    auto size_arr = new unsigned char[4];
    asio::async_read(_socket, asio::buffer(size_arr, 4),
                     [this, callback, size_arr](const asio::error_code &error,
                                                const std::size_t &length)
                     {
                         if (error) {
                             qDebug() << "connection::receive()" << error.message().c_str();
                             return;
                         }

                         std::size_t size = size_arr[0];
                         size += size_arr[1] << 8;
                         size += size_arr[2] << 16;
                         size += size_arr[3] << 24;
                         delete[] size_arr;

                         if (size <= 0 || size > 1024 * 1024) {
                             return;
                         }

                         auto message = new char[size];
                         asio::async_read(_socket, asio::buffer(message, size),
                                          [this, callback, message](const asio::error_code &error,
                                                                    const std::size_t &length)
                                          {
                                              if (error) {
                                                  qDebug() << "connection::receive()" << error.message().c_str();
                                                  return;
                                              }
                                              qDebug() << "connection::receive()" << length << "bytes";
                                              callback(length, message);
                                              receive(callback);
                                          });
                     });
}

void connection::send(const std::size_t &size, std::unique_ptr<char[]> &&data)
{
    _mutex.lock();

    bool empty_queue = _queue.empty();
    _queue.emplace_back(size, std::move(data));

    _mutex.unlock();

    if (empty_queue) write_next();
}

void connection::write_next()
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    if (!_socket.is_open()) return;
    if (_queue.empty()) return;

    auto &data = _queue.front();

    auto size_arr = new unsigned char[4];
    size_arr[0] = data.first & 0xFF;
    size_arr[1] = (data.first >> 8) & 0xFF;
    size_arr[2] = (data.first >> 16) & 0xFF;
    size_arr[3] = (data.first >> 24) & 0xFF;

    asio::async_write(_socket, asio::buffer(size_arr, 4),
                      [this, size_arr](const asio::error_code &error,
                                       const std::size_t &/*length*/)
                      {
                          delete[] size_arr;
                          if (error) {
                              qDebug() << error.message().c_str();
                              return;
                          }

                          std::lock_guard<std::mutex> lock_guard(_mutex);
                          auto &data = _queue.front();

                          asio::async_write(_socket, asio::buffer(data.second.get(), data.first),
                                            [this](const asio::error_code &error,
                                                   const std::size_t &length)
                                            {
                                                if (error) {
                                                    qDebug() << error.message().c_str();
                                                    return;
                                                }
                                                qDebug() << "connection::write()" << length << "bytes";
                                                _mutex.lock();
                                                _queue.pop_front();
                                                _mutex.unlock();
                                                write_next();
                                            });
                      });
}

