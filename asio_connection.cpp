#include "asio_connection.h"
#include "event_loop.h"

asio_connection::asio_connection(asio::io_service &io_service,
                       tcp::socket &&socket,
                       const tcp::resolver::query &query)
    : _io_service(io_service),
      _socket(std::move(socket)),
      _query(query),
      _buffer(_buffer_size)
{ }

asio_connection::asio_connection(asio::io_service &io_service, const tcp::resolver::query &query)
    : _io_service(io_service),
      _socket(io_service),
      _query(query),
      _buffer(_buffer_size)
{ }

asio_connection::asio_connection(asio_connection &&o)
    : _io_service(o._io_service),
      _socket(std::move(o._socket)),
      _query(o._query),
      _buffer(_buffer_size)
{ }

asio_connection::~asio_connection()
{
    close();
}

void asio_connection::connect(std::function<void()> callback)
{
    tcp::resolver resolver(_io_service);
    asio::async_connect(_socket, resolver.resolve(_query),
                        [this, callback](const asio::error_code &error,
                                         const tcp::resolver::iterator &itr)
                        {
//                             qDebug() << "connection::connect()" << error.message().c_str();
                            if (error) {
                                close();
                                if (disconnect_callback) disconnect_callback();
                                return;
                            }
                            callback();
                        });
}

void asio_connection::receive()
{
    asio::async_read(_socket, asio::buffer(_size_buffer_in, 4),
                     [this](const asio::error_code &error,
                                      const std::size_t &length)
                     {
                         if (error) {
//                              qDebug() << "connection::receive()" << error.message().c_str();
                             close();
                             if (disconnect_callback) disconnect_callback();
                             return;
                         }

                         std::size_t size = _size_buffer_in[0];
                         size += _size_buffer_in[1] << 8;
                         size += _size_buffer_in[2] << 16;
                         size += _size_buffer_in[3] << 24;

                         if (size > _buffer_size) {
//                              qDebug() << "connection::receive() invalid size:" << size;
                             close();
                             if (disconnect_callback) disconnect_callback();
                             return;
                         }

                         asio::async_read(_socket, asio::buffer(_buffer, size),
                                          [this](const asio::error_code &error,
                                                           const std::size_t &length)
                                          {
                                              if (error) {
//                                                   qDebug() << "connection::receive()" << error.message().c_str();
                                                  close();
                                                  if (disconnect_callback) disconnect_callback();
                                                  return;
                                              }
//                                               qDebug() << "connection::receive()" << length << "bytes";
                                              std::vector<byte> data(length);
                                              std::copy_n(_buffer.begin(), length, data.begin());
                                              default_event_loop.post([this, data]() { receive_callback(std::move(data)); });
                                              receive();
                                          });
                     });
}

void asio_connection::write_next()
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    if (!_socket.is_open()) return;
    if (_queue.empty()) return;
    if (_writing_queue) return;

    auto size = _queue.front().size();

    _size_buffer_out[0] = size & 0xFF;
    _size_buffer_out[1] = (size >> 8) & 0xFF;
    _size_buffer_out[2] = (size >> 16) & 0xFF;
    _size_buffer_out[3] = (size >> 24) & 0xFF;

    _writing_queue = true;
    asio::async_write(_socket, asio::buffer(_size_buffer_out, 4),
                      [this](const asio::error_code &error,
                             const std::size_t &/*length*/)
                      {
                          std::lock_guard<std::mutex> lock_guard(_mutex);

                          if (error) {
//                               qDebug() << "connection::write_next()" << error.message().c_str();
                              close();
                              if (disconnect_callback) disconnect_callback();
                              return;
                          }

                          auto &data = _queue.front();

                          asio::async_write(_socket, asio::buffer(data),
                                            [this](const asio::error_code &error,
                                                   const std::size_t &length)
                                            {
                                                _writing_queue = false;
                                                if (error) {
//                                                     qDebug() << "connection::write_next()" << error.message().c_str();
                                                    close();
                                                    if (disconnect_callback) disconnect_callback();
                                                    return;
                                                }
//                                                 qDebug() << "connection::write()" << length << "bytes";

                                                _mutex.lock();
                                                _queue.pop_front();
                                                _mutex.unlock();
                                                write_next();
                                            });
                      });
}

void asio_connection::set_disconnect_callback(std::function<void()> callback)
{
    disconnect_callback = callback;
}

void asio_connection::set_receive_callback(std::function<void(packet &&data)> callback)
{
    receive_callback = callback;
}

void asio_connection::send(packet &&data)
{
    _mutex.lock();
    bool empty_queue = _queue.empty();
    _queue.emplace_back(std::move(data));
    _mutex.unlock();
    if (empty_queue)
        write_next();
}

