#include "userdelegate.h"
#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

UserDelegate::UserDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

// adapted from http://stackoverflow.com/questions/1956542/how-to-make-item-view-render-rich-html-text-in-qt
void UserDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    painter->save();

    QTextDocument doc;
    doc.setHtml(options.text);

    options.text = "";
    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

    QSize iconSize = options.icon.actualSize(options.rect.size());
    painter->translate(options.rect.left()+iconSize.width(), options.rect.top());
    QRect clip(0, 0, options.rect.width()+iconSize.width(), options.rect.height());

    painter->setClipRect(clip);
    QAbstractTextDocumentLayout::PaintContext ctx;
    if (option.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, QColor("#ffffff"));
    ctx.clip = clip;
    doc.documentLayout()->draw(painter, ctx);

    painter->restore();
}
