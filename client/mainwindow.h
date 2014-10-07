#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <thread>
#include <QMainWindow>
#include <QModelIndex>

#include "../connect_layer.h"
#include "contactsmodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(asio::io_service &ioService, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_contacts_activated(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    ContactsModel *_contacts;
    asio::io_service &_io_service;
};

#endif // MAINWINDOW_H
