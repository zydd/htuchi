#include <QInputDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "userdelegate.h"
#include "packet_util.h"

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
    palette.setBrush(QPalette::Disabled, QPalette::Base, this->palette().color(QPalette::Background));
    ui->name->setPalette(palette);
    ui->status->setPalette(palette);

    QVariant username = _settings.value("username");
    if (username.isNull()) {
        bool ok;
        do {
            username = QInputDialog::getText(this, "Name", "Insert your name", QLineEdit::Normal, QString(), &ok);
        } while (username.isNull() && ok);

        if (!ok) throw std::runtime_error("username not set");
        _settings.setValue("username", username);
    }

    ui->name->setText(username.toString());
    _contacts->send_info(toPacket(username));

    connect(ui->name, SIGNAL(returnPressed()), SLOT(nameChanged()));
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
