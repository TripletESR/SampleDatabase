#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QtSql>
#include <QSqlQueryModel>
#include <QSqlRelationalTableModel>
#include <QVector>
#include <QMessageBox>
#include <QImage>
#include "constant.h"
#include "tableeditor.h"
#include "dateformatdelegate.h"
#include "openfiledelegate.h"


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
    void SetupSampleTableView();

    void on_pushButton_editChemical_clicked();
    void on_pushButton_editHost_clicked();
    void on_pushButton_editSolvent_clicked();

    void on_pushButton_sumbitSample_clicked();
    void on_pushButton_addSampleEntry_clicked();
    void on_pushButton_deleteSampleEntry_clicked();
    void on_pushButton_revertSample_clicked();


    void on_pushButton_editLaser_clicked();

private:
    Ui::MainWindow *ui;
    QSqlRelationalTableModel *sample;

    QSqlDatabase db;

    TableEditor *editorChemical;
    TableEditor *editorHost;
    TableEditor *editorSolvent;
    TableEditor *editorLaser;
};

#endif // MAINWINDOW_H
