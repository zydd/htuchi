#include <thread>

#include "../packet.h"

#include "chatwindow.h"
#include "ui_chatwindow.h"


ChatWindow::ChatWindow(int id)
    : ui(new Ui::ChatWindow),
      receiver_id(id)
{
    ui->setupUi(this);
    connect(ui->messageEdit, SIGNAL(returnPressed()), SLOT(send()));
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

void ChatWindow::send()
{
    if (!ui->messageEdit->text().isEmpty()) {
        packet msg(ui->messageEdit->text());
        msg.receiver_id = receiver_id;
        abstract_layer::processDown(std::move(msg));

        ui->messageBrowser->setTextColor(Qt::black);
        ui->messageBrowser->append("> " + ui->messageEdit->text());
        ui->messageEdit->clear();
    }
}

void ChatWindow::read_queue()
{
    show();
    while (!queue.isEmpty()) {
        std::lock_guard<std::mutex> lock_guard(_mutex);
        if (!queue.first().isEmpty()) {
            ui->messageBrowser->setTextColor(Qt::blue);
            ui->messageBrowser->append("< " + queue.first());
        }
        queue.pop_front();
    }
}

void ChatWindow::processUp(packet &&data)
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    QByteArray array((char *)data.data(), data.size());
    QDataStream ds(array);
    QVariant var;
    ds >> var;
    QString msg = var.toString();
    queue.append(msg);

    QMetaObject::invokeMethod(this, "read_queue", Qt::QueuedConnection);
}

