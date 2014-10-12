#include "connection_layer.h"

connection_layer::connection_layer()
{ }

connection_layer::~connection_layer()
{ }

void connection_layer::add_acceptor(acceptor &&acc)
{
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
    int id = _id++;
    qDebug() << "add_connection id" << id;
    auto empl = _connections.emplace(id, std::move(conn));

    if (empl.second) {
        connection &connection = empl.first->second;
        using std::placeholders::_1;
        using std::placeholders::_2;
        connection.receive(std::bind(&connection_layer::receive, this, id, _1, _2));
    }
}

void connection_layer::connect(connection &&conn)
{
    int id = _id++;
    auto empl = _connections.emplace(id, std::move(conn));

    if (empl.second) {
        auto itr = empl.first;
        connection &connection = itr->second;
        using std::placeholders::_1;
        using std::placeholders::_2;
        connection.connect([this, id, &connection](){connection.receive(std::bind(&connection_layer::receive, this, id, _1, _2));});
    }
}

void connection_layer::receive(int id, const std::size_t &size, char *data)
{
    default_event_loop.post([this, size, data]() {
        processIn({size, std::unique_ptr<char[]>(data)});
    });
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
            itr->second.send(data.size(), std::unique_ptr<char[]>(data.seriallize()));
        }
    } else {
        auto conn = _connections.find(data.id);
        if (conn != _connections.end())
            conn->second.send(data.size(), std::unique_ptr<char[]>(data.seriallize()));
    }
}
