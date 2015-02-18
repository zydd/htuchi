#ifndef CONTACTSMODEL_H
#define CONTACTSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QMutexLocker>
#include <QSettings>

#include "../packet.h"
#include "../client_manager.h"
#include "chatwindow.h"

class ContactsModel : public QAbstractListModel, public client_manager
{
    Q_OBJECT
public:
    struct User {
        enum Presence {Online, Away, Offline};

        int id;
        Presence presence;
        QString name;
        QString status;
    };

    ContactsModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    void update(const User &usr);
    virtual void processUp(packet &&data);
    virtual void build_above(int id);

private:
    QList<User> users;
    std::unordered_map<int, ChatWindow *> _chatwindows;
    mutable QMutex mutex;
    QSettings _settings;

public slots:
    void itemActivated(QModelIndex const& index);

private slots:
    ChatWindow *createChatWindow();
};

#endif // CONTACTSMODEL_H
