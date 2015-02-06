#include <mutex>

#include "connection_layer.h"

connection_layer::connection_layer()
{ }

connection_layer::~connection_layer()
{ }

void connection_layer::add_acceptor(asio_acceptor &&acc)
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    int id = _id++;
    auto empl = _acceptors.emplace(id, std::move(acc));
    if (empl.second) {
        asio_acceptor &acceptor = empl.first->second;
        using std::placeholders::_1;
        acceptor.accept(std::bind(&connection_layer::add_connection, this, _1));
    }
}

void connection_layer::add_connection(asio_connection &&conn)
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    int id = _id++;
    qDebug() << "add_connection() id:" << id;
    auto empl = _connections.emplace(id, std::move(conn));

    if (empl.second) {
        asio_connection &connection = empl.first->second;
        connection.set_disconnect_callback(std::bind(&connection_layer::disconnected, this, id));
        using std::placeholders::_1;
	connection.set_receive_callback(std::bind(&connection_layer::receive, this, id, _1));
        if (connection.is_open()) {
            connection.receive();
            connection.write_next();
        } else {
            connection.connect([this, id, &connection]() {
                std::lock_guard<std::mutex> lock_guard(_mutex);
                connection.receive();
                connection.write_next();
            });
        }
    }
}

void connection_layer::disconnected(int id)
{
    auto conn = _connections.find(id);
    if (conn != _connections.end()) // TODO: try reconnect
        _connections.erase(conn);
}

void connection_layer::receive(int id, packet &&data)
{
//     data.push((unsigned char) (id & 0xFF)); // !!Trunc!!
    processDown(std::move(data));
}

