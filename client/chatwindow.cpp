#include <thread>

#include "chatwindow.h"
#include "ui_chatwindow.h"


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

void ChatWindow::send()
{
    if (!ui->messageEdit->text().isEmpty()) {
        QVariant msg(ui->messageEdit->text());
        default_event_loop.post([this, msg](){
            processOut(msg);
        });
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

void ChatWindow::processIn(packet &&data)
{
    std::lock_guard<std::mutex> lock_guard(_mutex);
    QByteArray array(data.data(), data.size());
    QDataStream ds(array);
    QVariant var;
    ds >> var;
    QString msg = var.toString();
    queue.append(msg);

    QMetaObject::invokeMethod(this, "read_queue", Qt::QueuedConnection);
}

void ChatWindow::processOut(packet &&data)
{
    if (_below) _below->processOut(std::move(data));
}
