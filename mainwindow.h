#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QtSql>
#include <QVector>
#include "constant.h"
#include "tableeditor.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QStringList GetTableColEntries(QString tableName, int col);
    int GetTableColNumber(QString tableName);
    void ShowTable(QString tableName);

private slots:

    void on_pushButton_EditEntry_clicked();
    void updateCombox(QString tableName);

    void on_pushButton_SelectSample_clicked();

private:
    Ui::MainWindow *ui;
    QSqlTableModel *sample;
    QSqlTableModel *data;

    QSqlDatabase db;
    TableEditor *editor;
};

#endif // MAINWINDOW_H
