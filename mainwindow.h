#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QtSql>
#include <QVector>
#include "constant.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void AddEntry(QString tableName, QVector<QString> values);
    void DeleteEntry(QString tableName, int entry_ID);
    void ShowTable(QString tableName);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
