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
    connect(ui->contacts, SIGNAL(activated(QModelIndex)), _contacts, SLOT(itemActivated(QModelIndex)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

