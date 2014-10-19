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

#include "mainwindow.h"
#include "chatwindow.h"
#include "../sodium_secret_layer.h"

int main(int argc, char *argv[])
{
    if (sodium_init() < 0)
        throw std::runtime_error("sodium_init() failed");

    QApplication a(argc, argv);

    std::string pass = QInputDialog::getText(nullptr, "Password", "Password", QLineEdit::Password).toStdString();
    if (pass.empty()) return 0;
    unsigned char key[crypto_secretbox_KEYBYTES];
    crypto_hash_sha256(key, (unsigned char *)pass.c_str(), pass.size());

    asio::io_service io_service;
    asio::io_service::work work(io_service);
    std::thread thread([&io_service](){io_service.run();});
    std::thread thread2([](){default_event_loop.run();});

    connection_layer conn;
    conn.add_connection({io_service, {"localhost", "48768"}});

    sodium_secret_layer enc(key);
    conn.insertAbove(&enc);

    ChatWindow w;
    enc.insertAbove(&w);

    w.setWindowTitle("Client");
    w.show();
    a.exec();

    io_service.stop();
    default_event_loop.stop();
    thread.join();
    thread2.join();
    return 0;
}


