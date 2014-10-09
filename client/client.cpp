/*
 * +--------+----------------+
 * |        | Encryption     |
 * |        +----------------+
 * | Server | Addressing     |
 * |        +----------------+
 * |        | Delivery       |
 * +--------+----------------+
 * |        | Encryption     |
 * |        +----------------+
 * |        | Compression    |
 * |        +----------------+
 * | Client | Authentication |
 * |        +----------------+
 * |        | Delivery       |
 * |        +----------------+
 * |        | Message        |
 * +--------+----------------+
 */

#include <QApplication>
#include <asio.hpp>

#include "mainwindow.h"
#include "chatwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    asio::io_service io_service;
    asio::io_service::work work(io_service);
    std::thread thread([&io_service](){io_service.run();});
    std::thread thread2([](){default_event_loop.run();});

    connection_layer<> conn(io_service);
    conn.connect({"localhost", "4000"});
    ChatWindow w;

    conn.insertAbove(&w);

    w.setWindowTitle("Client");
    w.show();
    a.exec();

    conn.close();
    io_service.stop();
    default_event_loop.stop();
    thread.join();
    thread2.join();
    return 0;
}
