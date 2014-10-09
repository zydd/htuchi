#ifndef CONNECTION_LAYER_CPP
#define CONNECTION_LAYER_CPP

#include "connection_layer.h"

template<typename Socket>
connection_layer<Socket>::connection_layer(asio::io_service &io_service)
    : _io_service(io_service),
      _socket(io_service)
{ }

template<typename Socket>
connection_layer<Socket>::~connection_layer()
{ }

template<typename Socket>
void connection_layer<Socket>::_listen(const tcp::endpoint &endpoint)
{
    int id = ++_id;
    _acceptors.emplace(id, tcp::acceptor(_io_service, endpoint));
    accept(id);
}

template<typename Socket>
void connection_layer<Socket>::accept(int id)
{
    auto acc = _acceptors.find(id);
    if (acc == _acceptors.end()) return;

    tcp::acceptor &acceptor = acc->second;

    acceptor.async_accept(_socket, [this, id](const asio::error_code &error)
    {
        if (error) throw std::runtime_error("connection_layer::accept() " + error.message());
        int id_sock = ++_id;
        auto conn = _connections.emplace(id_sock, std::move(_socket));
        receive(id_sock);
        send();
        accept(id);
    });
}


template<typename Socket>
void connection_layer<Socket>::_connect(const tcp::resolver::query &query)
{
    int id = ++_id;
    auto conn = _connections.emplace(id, Socket(_io_service));
    _queries.emplace(id, query);
    Socket &socket = conn.first->second;

    static tcp::resolver resolver(_io_service);

    asio::async_connect(socket, resolver.resolve(query),
                        [this, id](const asio::error_code &error,
                                   const tcp::resolver::iterator &/*itr*/)
    {
        if (error) throw std::runtime_error("connection_layer::connect() " + error.message());
        receive(id);
        send();
    });
}

template<typename Socket>
void connection_layer<Socket>::_close()
{

}

template<typename Socket>
void connection_layer<Socket>::_close(int id)
{
    if (id == -1) return;
    Socket &socket = _connections.find(id)->second;
    if (socket.is_open()) {
        socket.shutdown(Socket::shutdown_send);
        socket.close();
    }
    _connections.erase(id);
    _acceptors.erase(id);
    _queries.erase(id);
}


template<typename Socket>
void connection_layer<Socket>::receive(int id)
{
    Socket &socket = _connections.find(id)->second;
    if (!socket.is_open()) return;

    auto size_arr = new unsigned char[4];
    asio::async_read(socket, asio::buffer(size_arr, 4),
                     [this, id, &socket, size_arr](const asio::error_code &error,
                                                   const std::size_t &length)
    {
        if (error) {
            qDebug() << "connection_layer::receive()" << error.message().c_str();
            _close(id);
            return;
        }

        std::size_t size = size_arr[0];
        size += size_arr[1] << 8;
        size += size_arr[2] << 16;
        size += size_arr[3] << 24;
        delete[] size_arr;

        if (size > 1024 * 1024) {
            _close(id);
            return;
        }
        auto message = new char[size];
        asio::async_read(socket, asio::buffer(message, size),
                         [this, id, message, size](const asio::error_code &error,
                                                   const std::size_t &length)
        {
            if (error) {
                qDebug() << "connection_layer::receive()" << error.message().c_str();
                _close(id);
                return;
            }
            qDebug() << "connection_layer::receive()" << length << "bytes id:" << id;
            default_event_loop.post([this, id, message, size](){
                packet data(std::unique_ptr<char[]>(message), size);
                data.id = id;
                processIn(std::move(data));
            });

            receive(id);
        });
    });
}

template<typename Socket>
void connection_layer<Socket>::send()
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    if (_queue.empty()) return;
    if (_connections.empty()) return;

    auto &data = _queue.front();
    auto conn = _connections.find(std::get<0>(data));
    if (conn == _connections.end()) return;

    Socket &socket = conn->second;
    if (!socket.is_open()) return;

    auto size = new unsigned char[4];
    size[0] = std::get<2>(data) & 0xFF;
    size[1] = (std::get<2>(data) >> 8) & 0xFF;
    size[2] = (std::get<2>(data) >> 16) & 0xFF;
    size[3] = (std::get<2>(data) >> 24) & 0xFF;

    asio::async_write(socket, asio::buffer(size, 4),
                      [this, &socket, &data, size](const asio::error_code &error,
                                                   const std::size_t &/*length*/)
    {
        delete[] size;
        if (error) {
            qDebug() << error.message().c_str();
            _close(std::get<0>(data));
            return;
        }

        asio::async_write(socket, asio::buffer(std::get<1>(data), std::get<2>(data)),
                          [this, &socket, &data](const asio::error_code &error,
                                                 const std::size_t &length)
        {
            if (error) {
                qDebug() << error.message().c_str();
                _close(std::get<0>(data));
                return;
            }
            qDebug() << "connection_layer::send()" << length << "bytes id:" << std::get<0>(data);
            _mutex.lock();
            _queue.pop_front();
            _mutex.unlock();
            send();
        });
    });
}

template<typename Socket>
void connection_layer<Socket>::processIn(packet &&data)
{
    if (_above) _above->processIn(std::move(data));
}

template<typename Socket>
void connection_layer<Socket>::processOut(packet &&data)
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    bool empty_queue = _queue.empty();

    if (data.id == -1) {
        for (auto itr = _connections.begin(), end = _connections.end(); itr != end; ++itr)
            _queue.emplace_back(std::make_tuple(itr->first, data.seriallize(), data.size()));
    } else
        _queue.emplace_back(std::make_tuple(data.id, data.seriallize(), data.size()));

    if (empty_queue)
        _io_service.post(std::bind(&connection_layer<Socket>::send, this));
}

#endif // CONNECTION_LAYER_CPP
