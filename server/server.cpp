#include <thread>
#include <QApplication>
#include <asio.hpp>

#include "../client/chatwindow.h"
#include "../connection_layer.h"
#include "../reflector_layer.h"
#include "../event_loop.h"

using asio::ip::tcp;

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    asio::io_service io_service;
    std::thread thread([&io_service](){default_event_loop.run();});

    connection_layer conn;
    conn.add_acceptor(acceptor(io_service, tcp::endpoint(tcp::v4(), 4000)));

    reflector_layer mirror;
    conn.insertAbove(&mirror);

    io_service.run();
    default_event_loop.stop();
    thread.join();

    return 0;
}
