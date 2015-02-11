#include <QIcon>
#include <QBrush>

#include "contactsmodel.h"

ContactsModel::ContactsModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

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
    emit dataChanged(QModelIndex(),QModelIndex());  // TODO: Do it the righ way!
}

void ContactsModel::processUp(packet &&data)
{
    if (data.empty()) return;

    byte flags = data[data.size() - 1];

    client_manager::processUp(std::move(data));

    if (flags & List) {
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
    }
}
