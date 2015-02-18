#include <thread>
#include <QDebug>

#include "packet_util.h"
#include "chatwindow.h"
#include "ui_chatwindow.h"


ChatWindow::ChatWindow(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::ChatWindow)
{
    ui->setupUi(this);
    connect(ui->messageEdit, SIGNAL(returnPressed()), SLOT(send()));
    show();  // TODO: find out why I get *** corrupted double-linked list *** without this line
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

void ChatWindow::send()
{
    if (!ui->messageEdit->text().isEmpty()) {
        packet msg(toPacket(ui->messageEdit->text()));
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

    queue.append(toVariant(data).toString());

    QMetaObject::invokeMethod(this, "read_queue", Qt::QueuedConnection);
}

