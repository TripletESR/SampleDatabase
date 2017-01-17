#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DB_PATH);
    qDebug() << db.databaseName();
    qDebug() << db.open();

    QStringList tableList = db.tables();

    qDebug() << tableList;

    ShowTable(tableList[0]);

    QStringList hostList = GetTableColEntries(tableList[0], 0);
    ui->comboBox_1->addItems(hostList);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::AddEntry(QString tableName, QVector<QString> values)
{
    int colSize = GetTableColNumber(tableName);
    if( values.size() != colSize) return;

    QString val, temp;
    val.sprintf("(%s,", values[0]);
    for(int i = 1; i < colSize-1; i++){
        temp.sprintf("%s,", values[i]);
        val += temp;
    }
    temp.sprintf("%s)", values[colSize-1]);
    val += temp;

    QString cmd;
    cmd = "INSERT INTO " + tableName + "VALUES" + val;

    qDebug() << cmd;

    QSqlQuery query;
    query.exec(cmd);

}

QStringList MainWindow::GetTableColEntries(QString tableName, int col)
{
    QSqlQuery query;
    query.exec("SELECT *FROM " + tableName);

    QStringList entries;

    while(query.next()){
        entries << query.value(col).toString();
    }

    return entries;
}

int MainWindow::GetTableColNumber(QString tableName)
{
    QSqlQuery query;
    query.exec("SELECT *FROM " + tableName);
    return query.record().count();
}

void MainWindow::ShowTable(QString tableName)
{
    QSqlQuery query;
    query.exec("SELECT *FROM " + tableName);

    QString msg, temp;

    int col = query.record().count();
    query.last();
    int row = query.at() + 1;

    msg.sprintf("Table Name : %s, size = %d x %d", tableName.toStdString().c_str(), col, row);
    qDebug() << msg;
    msg.clear();

    QStringList fieldNameList;
    for(int j = 0; j< col; j++){
        fieldNameList << query.record().fieldName(j);
    }

    qDebug() << fieldNameList;

    query.first();
    query.previous();
    while(query.next()){
        for(int j = 0; j< col; j++){
            fieldNameList << query.record().fieldName(j);
            temp = query.value(j).toString();
            msg += temp;
            msg += " | ";
        }
        qDebug() << msg;
        msg.clear();
    }

}

void MainWindow::on_pushButton_EditEntry_clicked()
{
    TableEditor *editor = new TableEditor("Host");
    editor->show();
}
