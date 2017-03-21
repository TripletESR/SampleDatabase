#ifndef OPENFILEDELEGATE_H
#define OPENFILEDELEGATE_H

#include <QStyledItemDelegate>
#include <QFileDialog>
#include <QDebug>

class OpenFileDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    OpenFileDelegate(QObject * parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    //QWidget *createEditor(QWidget *parent) const;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

#endif // OPENFILEDELEGATE_H
