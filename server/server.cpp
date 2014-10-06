#include <thread>
#include <QApplication>
#include <asio.hpp>

#include "../client/chatwindow.h"
#include "../connection_layer.h"
#include "../crossover_layer.h"
#include "../event_loop.h"

using asio::ip::tcp;

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    asio::io_service io_service;
    asio::io_service::work work(io_service);
    std::thread thread([&io_service](){io_service.run();});
//     std::thread thread2([](){main_event_loop.run();});

    connection_layer conn(io_service);
    crossover_layer cross;
    conn.insertAbove(&cross);
    cross.insertAbove(&conn);
//     ChatWindow w;
//     w.insertBelow(&cross);
//     cross.insertBelow(&w);

//     conn.insertAbove(&w);
    conn.accept({tcp::v4(), 4000});
//     conn.receive();

//     w.setWindowTitle("Server");
//     w.show();
//     a.exec();
    qDebug() << "starting event_loop";
    main_event_loop.run();
    conn.close();
    thread.join();
    main_event_loop.stop();
//     thread2.join();
    return 0;
}
