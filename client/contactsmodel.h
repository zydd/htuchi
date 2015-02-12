#ifndef CONTACTSMODEL_H
#define CONTACTSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QMutexLocker>

#include "../packet.h"
#include "../client_manager.h"


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
    mutable QMutex mutex;

public slots:
    void itemActivated(QModelIndex const& index);

private slots:
    void createChatWindow(int id);
};

#endif // CONTACTSMODEL_H
