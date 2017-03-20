#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QtSql>
#include <QSqlQueryModel>
#include <QVector>
#include <QSqlRelationalTableModel>
#include <QMessageBox>
#include <QDataWidgetMapper>
#include "constant.h"
#include "tableeditor.h"
#include "dateformatdelegate.h"


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

    void on_comboBox_chemical_currentTextChanged(const QString &arg1);
    void on_pushButton_editChemical_clicked();

    void updateChemicalCombox(QString tableName);

    void on_pushButton_selectSample_clicked();
    void on_pushButton_sumbitSample_clicked();
    void on_pushButton_addSampleEntry_clicked();
    void on_pushButton_deleteSampleEntry_clicked();
    void on_pushButton_revertSample_clicked();

    void on_pushButton_submitData_clicked();
    void on_pushButton_addDataEntry_clicked();
    void on_pushButton_deleteDataEntry_clicked();
    void on_pushButton_revertData_clicked();

    void on_pushButton_open_clicked();

private:
    Ui::MainWindow *ui;
    QSqlRelationalTableModel *sample;
    QSqlTableModel *data;

    QSqlDatabase db;

    TableEditor *editor;
};

#endif // MAINWINDOW_H
