#include <QIcon>
#include <QBrush>
#include <QApplication>
#include <QThread>
#include <QDebug>
#include <thread>

#include "contactsmodel.h"
#include "chatwindow.h"
#include "../sodium_secret_layer.h"

ContactsModel::ContactsModel(QObject *parent)
    : QAbstractListModel(parent),
      _settings("zydd", "htuchi")
{ }

int ContactsModel::rowCount(const QModelIndex &/*parent*/) const
{
    QMutexLocker lock(&mutex);
    return users.count();
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const
{
    QMutexLocker lock(&mutex);
    if (!index.isValid())
        return QVariant();

    if (index.row() >= users.size())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        switch (users.at(index.row()).presence) {
        case User::Offline:
            return "<font color=gray>" + users.at(index.row()).name;
        case User::Online:
        case User::Away:
            return users.at(index.row()).name + (!users.at(index.row()).status.isEmpty() ?
                        "<font color=#c0c0c0> - <font size=2><i>" + users.at(index.row()).status
                      :
                         "");
        }

    case Qt::DecorationRole:
        switch (users.at(index.row()).presence) {
        case User::Online:  return QIcon(":/res/img/online.png");
        case User::Offline: return QIcon(":/res/img/offline.png");
        case User::Away:    return QIcon(":/res/img/away.png");
        }

    case Qt::SizeHintRole:  return QSize(0, 25);

    default: return QVariant();
    }
}

void ContactsModel::update(const User &usr) {
    QMutexLocker lock(&mutex);
    for (int i = 0; i < users.size(); ++i) {
        if (users[i].id == usr.id) {
            users[i] = usr;
            return;
        }
    }
    users.append(usr);
}

void ContactsModel::processUp(packet &&data)
{
    if (data.empty()) return;

    byte flags = data[data.size() - 1];

    client_manager::processUp(std::move(data));

    if (flags & List) {
        users.clear();
        for (auto const& client : _clients) {
            std::vector<byte> const& info = client.second.info;
            if (!info.empty()) {
                QByteArray array((char *)info.data(), info.size());
                QDataStream ds(array);
                QVariant var;
                ds >> var;
                update({client.first, User::Online, var.toString(), ""});
            }
        }
        emit dataChanged(QModelIndex(),QModelIndex());  // TODO: Do it the righ way!
    }
}

void ContactsModel::build_above(int id)
{
    ChatWindow *wnd = nullptr;
    if (qApp->thread() == QThread::currentThread())
        wnd = new ChatWindow;
    else
        QMetaObject::invokeMethod(this, "createChatWindow", Qt::BlockingQueuedConnection, Q_RETURN_ARG(ChatWindow*, wnd));
    if (!wnd) return;

    auto client = _clients.find(id);
    if (client == _clients.end()) return;

    QByteArray key = _settings.value("key").toByteArray();
    if (key.size() != crypto_secretbox_KEYBYTES) return;
    unsigned char *pkey = new unsigned char[key.size()];
    std::copy_n(key.begin(), crypto_secretbox_KEYBYTES, pkey);
    auto enc = new sodium_secret_layer(std::unique_ptr<unsigned char>(pkey));

    enc->setAbove(wnd);
    wnd->setBelow(enc);
    _above[id] = enc;
    _chatwindows[id] = wnd;
    client_manager::build_above(id);
}

ChatWindow *ContactsModel::createChatWindow()
{
    return new ChatWindow;
}

void ContactsModel::itemActivated(const QModelIndex& index)
{
    if (!index.isValid() || index.row() >= users.size())
        return;

    int id = users[index.row()].id;

    auto cw = _chatwindows.find(id);
    if (cw == _chatwindows.end())
        build_above(id);

    cw = _chatwindows.find(id);
    if (cw != _chatwindows.end() && cw->second)
        cw->second->show();
}

