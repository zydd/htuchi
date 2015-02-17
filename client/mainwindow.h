#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <thread>
#include <QMainWindow>
#include <QModelIndex>
#include <QSettings>

#include "../connection_layer.h"
#include "contactsmodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ContactsModel *_contacts;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSettings _settings;


private slots:
    void nameChanged();
    void statusChanged();
    void setInfo();
    void options();
};

#endif // MAINWINDOW_H
