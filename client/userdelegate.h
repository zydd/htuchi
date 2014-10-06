#ifndef USERDELEGATE_H
#define USERDELEGATE_H

#include <QStyledItemDelegate>

class UserDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit UserDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem& option,
               const QModelIndex &index) const;

signals:

public slots:

};

#endif // USERDELEGATE_H
