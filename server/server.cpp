#include <thread>
#include <QApplication>
#include <asio.hpp>

#include "../client/chatwindow.h"
#include "../listen_layer.h"
#include "../crossover_layer.h"
#include "../event_loop.h"

using asio::ip::tcp;

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    asio::io_service io_service;
    std::thread thread([&io_service](){main_event_loop.run();});

    listen_layer conn(io_service, {tcp::v4(), 4000});
    crossover_layer cross;
    conn.insertAbove(&cross);
    cross.insertAbove(&conn);

    io_service.run();
    main_event_loop.stop();
    thread.join();

    return 0;
}
