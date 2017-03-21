#include "openfiledelegate.h"

OpenFileDelegate::OpenFileDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

QWidget *OpenFileDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QFileDialog * editor = new QFileDialog(parent);

    editor->setReadOnly(true);
    QStringList filters;
    filters << "Data File (*txt *dat *csv *.*)";
    editor->setNameFilters(filters);


    return editor;
}

void OpenFileDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QFileDialog *fileDialog = qobject_cast<QFileDialog *> (editor);
    QStringList fileNameList = fileDialog->selectedFiles();
    model->setData(index, fileNameList[0]);
}

void OpenFileDelegate::updateEditorGeometry(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //qDebug() << parent->geometry();
    //qDebug() << parent->parentWidget()->parentWidget()->parentWidget()->parentWidget()->parentWidget()->pos();
    QRect rect = parent->parentWidget()->parentWidget()->parentWidget()->parentWidget()->parentWidget()->geometry();
    parent->setGeometry(rect.x()+rect.width()/4., rect.y()+rect.height()/4., rect.width(), rect.height()*3./4.);
}
