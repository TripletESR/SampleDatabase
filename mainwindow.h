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
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QCloseEvent>
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
    void SaveTable(QString tableName, QString showName, QTextStream &stream);

    void ShowTable(QString tableName);

private slots:
    void closeEvent(QCloseEvent *event);
    void ErrorAndClose(QString msg);

    void SetupSampleTableView();
    void SetupESRDataTableView();
    void SetupNMRDataTableView();
    int loadConfigurationFile();

    void on_pushButton_editChemical_clicked();
    void on_pushButton_editSolvent_clicked();
    void on_pushButton_editLaser_clicked();

    void on_pushButton_sumbitSample_clicked();
    void on_pushButton_addSampleEntry_clicked();
    void on_pushButton_deleteSampleEntry_clicked();
    void on_pushButton_revertSample_clicked();

    void on_pushButton_addDataEntry_clicked();
    void on_pushButton_deleteDataEntry_clicked();
    void on_pushButton_revertData_clicked();
    void on_pushButton_submitData_clicked();

    void on_actionOutput_tables_triggered();

    void on_radioButton_ESRData_clicked();

    void on_radioButton_NMRData_clicked();

private:
    Ui::MainWindow *ui;
    QSqlRelationalTableModel *sample;
    QSqlRelationalTableModel *data;

    QSqlDatabase db;

    TableEditor *editorChemical;
    TableEditor *editorSolvent;
    TableEditor *editorLaser;
};

#endif // MAINWINDOW_H
