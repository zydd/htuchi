#include "connection_layer.h"

connection_layer::connection_layer()
{ }

connection_layer::~connection_layer()
{ }

void connection_layer::add_acceptor(acceptor &&acc)
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    int id = _id++;
    auto empl = _acceptors.emplace(id, std::move(acc));
    if (empl.second) {
        acceptor &acceptor = empl.first->second;
        using std::placeholders::_1;
        acceptor.accept(std::bind(&connection_layer::add_connection, this, _1));
    }
}

void connection_layer::add_connection(connection &&conn)
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    int id = _id++;
    qDebug() << "add_connection() id:" << id;
    auto empl = _connections.emplace(id, std::move(conn));

    if (empl.second) {
        using std::placeholders::_1;
        connection &connection = empl.first->second;
        connection.set_disconnect_callback(std::bind(&connection_layer::disconnected, this, id));
        if (connection.is_open())
            connection.receive(std::bind(&connection_layer::receive, this, id, _1));
        else
            connection.connect([this, id, &connection]() {
                std::lock_guard<std::mutex> lock_guard(_mutex);
                connection.receive(std::bind(&connection_layer::receive, this, id, _1));
            });
    }
}

void connection_layer::receive(int id, std::vector<byte> &&data)
{
    default_event_loop.post([this, id, data]() {
        packet pack(std::move(data));
        pack.id = id;
        processIn(std::move(pack));
    });
}

void connection_layer::disconnected(int id)
{
    auto conn = _connections.find(id);
    if (conn != _connections.end()) // TODO: try reconnect
        _connections.erase(conn);
}

void connection_layer::processIn(packet &&data)
{
    if (_above) _above->processIn(std::move(data));
}

void connection_layer::processOut(packet &&data)
{
    std::lock_guard<std::mutex> lock_guard(_mutex);

    if (data.id == -1) {
        for (auto itr = _connections.begin(), end = _connections.end(); itr != end; ++itr) {
            packet pack(data);
            itr->second.send(std::move(pack));
        }
    } else {
        auto conn = _connections.find(data.id);
        if (conn != _connections.end()) {
            conn->second.send(std::move(data));
        }
    }
}
