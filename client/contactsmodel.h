#ifndef CONTACTSMODEL_H
#define CONTACTSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QMutexLocker>

class ContactsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    struct User {
        enum Presence {Online, Away, Offline};

        u_int8_t id;
        Presence presence;
        QString name;
        QString status;
    };

    ContactsModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    void update(const User &usr);

private:
    QList<User> users;
    mutable QMutex mutex;
};

#endif // CONTACTSMODEL_H
