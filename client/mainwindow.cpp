#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "userdelegate.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      _contacts(new ContactsModel)
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
