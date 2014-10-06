#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "userdelegate.h"

MainWindow::MainWindow(asio::io_service &io_service, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _contacts(new ContactsModel),
    _io_service(io_service)
{
    ui->setupUi(this);
    ui->contacts->setModel(_contacts);
    ui->contacts->setItemDelegate(new UserDelegate);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_contacts_activated(const QModelIndex &/*index*/)
{

}
