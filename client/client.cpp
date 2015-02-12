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
#include <QInputDialog>
#include <asio.hpp>
#include <sodium.h>

#include "../packet.h"

#include "mainwindow.h"
#include "chatwindow.h"
#include "../sodium_secret_layer.h"
#include "../client_manager.h"

int main(int argc, char *argv[])
{
    if (sodium_init() < 0)
        throw std::runtime_error("sodium_init() failed");

    QApplication a(argc, argv);

    asio::io_service io_service;
    asio::io_service::work work(io_service);
    std::thread thread([&io_service](){io_service.run();});
    std::thread thread2([](){default_event_loop.run();});

//     QString pass = QInputDialog::getText(nullptr, "Password", "", QLineEdit::Password);
//     packet p(pass);
//     if (pass.isEmpty()) return 0;
//     unsigned char key[crypto_secretbox_KEYBYTES];
//     crypto_hash_sha256(key, p.data(), p.size());

    connection_layer conn;
    conn.add_connection({io_service, {"localhost", "48768"}});

    auto wnd = new MainWindow();

    conn.setAbove(wnd->_contacts);
    wnd->_contacts->setBelow(&conn);

    wnd->_contacts->set_info(packet(QString("Client")));

    wnd->show();
    a.exec();

    io_service.stop();
    default_event_loop.stop();
    thread.join();
    thread2.join();
    return 0;
}


