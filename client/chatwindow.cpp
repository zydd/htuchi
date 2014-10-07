#include <thread>
#include "chatwindow.h"
#include "ui_chatwindow.h"
#include <QDebug>
#include <qbuffer.h>
ChatWindow::ChatWindow()
    : ui(new Ui::ChatWindow)
{
    ui->setupUi(this);
    connect(ui->messageEdit, SIGNAL(returnPressed()), SLOT(send()));
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

void ChatWindow::processIn(const packet &data)
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    QByteArray array(data.seriallize(), data.size());
    QDataStream ds(array);
    QVariant var;
    ds >> var;
    QString msg = var.toString();
    queue.append(msg);

    QMetaObject::invokeMethod(this, "read_queue", Qt::QueuedConnection);
}

void ChatWindow::processOut(const packet &data)
{
    if (_below) _below->processOut(data);
}

void ChatWindow::send()
{
    if (!ui->messageEdit->text().isEmpty()) {
        main_event_loop.post([this](){processOut(QVariant(ui->messageEdit->text()));});
        ui->messageBrowser->append(ui->messageEdit->text());
        ui->messageEdit->clear();
    }
}

void ChatWindow::read_queue()
{
    while (!queue.isEmpty()) {
        std::lock_guard<std::mutex> lock_guard(_mutex);
        ui->messageBrowser->append(queue.first());
        queue.pop_front();
    }
}
