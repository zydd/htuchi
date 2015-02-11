#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QDialog>
#include <QList>
#include <mutex>

#include "../abstract_layer.h"

namespace Ui
{
class ChatWindow;
}

class ChatWindow : public QDialog, public abstract_layer
{
    Q_OBJECT
public:
    ChatWindow(int id);
    ~ChatWindow();
    virtual void processUp(packet &&data);

private slots:
    void send();
    void read_queue();

private:
    Ui::ChatWindow* ui;
    QStringList queue;
    std::mutex _mutex;
    int receiver_id;
};

#endif // CHATWINDOW_H
