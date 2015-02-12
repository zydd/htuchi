#include <thread>
#include <functional>
#include <asio.hpp>

#include "../connection_layer.h"
#include "../reflector_layer.h"
#include "../event_loop.h"
#include "../client_manager.h"

using asio::ip::tcp;

int main(int argc, char **argv)
{
    asio::io_service io_service;
    std::thread thread([&io_service](){default_event_loop.run();});

    connection_layer conn;
    conn.add_acceptor(asio_acceptor(io_service, tcp::endpoint(tcp::v4(), 48768)));

    client_manager cm;

    conn.setAbove(&cm);
    cm.setBelow(&conn);

    using std::placeholders::_1;
    using std::placeholders::_2;
    conn.set_connection_change(std::bind(&client_manager::update_client_status, &cm, _1, _2));

    io_service.run();
    default_event_loop.stop();
    thread.join();

    return 0;
}
