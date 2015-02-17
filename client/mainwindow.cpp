#include <QInputDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "userdelegate.h"
#include "packet_util.h"
#include "optionswindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      _contacts(new ContactsModel),
      _settings("zydd", "htuchi")
{
    ui->setupUi(this);
    ui->contacts->setModel(_contacts);
    ui->contacts->setItemDelegate(new UserDelegate);
    connect(ui->contacts, SIGNAL(activated(QModelIndex)), _contacts, SLOT(itemActivated(QModelIndex)));

    QPalette palette;
    palette.setBrush(QPalette::Active, QPalette::Base, this->palette().color(QPalette::Background));
    palette.setBrush(QPalette::Inactive, QPalette::Base, this->palette().color(QPalette::Background));
    palette.setBrush(QPalette::Disabled, QPalette::Base, this->palette().color(QPalette::Background));
    ui->name->setPalette(palette);
    ui->status->setPalette(palette);

    QVariant username = _settings.value("username");
    if (username.isNull())
        options();
    else
        setInfo();

    connect(ui->name, SIGNAL(returnPressed()), SLOT(nameChanged()));

    connect(ui->toolButton, SIGNAL(pressed()), SLOT(options()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::nameChanged()
{
    if (ui->name->text().isEmpty())
        ui->name->setText(_settings.value("username").toString());
    else {
        QString username = ui->name->text();
        _settings.setValue("username", username);
        _contacts->send_info(toPacket(username));
    }
}

void MainWindow::statusChanged()
{

}

void MainWindow::options()
{
    OptionsWindow opt;
    connect(&opt, SIGNAL(infoUpdated()), SLOT(setInfo()));
    opt.exec();
}

void MainWindow::setInfo()
{
    QVariant username = _settings.value("username");
    ui->name->setText(username.toString());
    _contacts->send_info(toPacket(username));
}

