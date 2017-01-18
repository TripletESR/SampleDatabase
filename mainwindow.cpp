#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    newSampleDialog = new NewSampleDialog(this);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DB_PATH);
    //qDebug() << db.databaseName();
    qDebug() << db.open();
    QStringList tableList = db.tables();
    qDebug() << tableList;

    sample = new QSqlTableModel(this);
    sample->setTable(tableList[4]);
    sample->setEditStrategy(QSqlTableModel::OnManualSubmit);
    sample->select();
    ui->sampleView->setModel(sample);
    ui->sampleView->resizeColumnsToContents();

    data = new QSqlTableModel(this);
    data->setTable(tableList[3]);
    data->setEditStrategy(QSqlTableModel::OnManualSubmit);
    data->select();
    ui->dataView->setModel(data);
    ui->dataView->resizeColumnsToContents();

    editor = NULL;

    ShowTable(tableList[4]);

    updateCombox(tableList[0]);

    qDebug() << sample->fieldIndex("Name");

}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::updateCombox(QString tableName)
{
    QStringList hostList = GetTableColEntries(tableName, 0);
    ui->comboBox_1->clear();
    ui->comboBox_1->addItems(hostList);
}

void MainWindow::on_pushButton_EditEntry_clicked()
{
    editor = new TableEditor("Chemical");
    disconnect(editor);
    connect(editor, SIGNAL(closed(QString)), this, SLOT(updateCombox(QString)));
    editor->show();
}


void MainWindow::on_pushButton_SelectSample_clicked()
{
    QItemSelectionModel *selmodel = ui->sampleView->selectionModel();
    QModelIndex current = selmodel->currentIndex(); // the "current" item

    qDebug() << current;

}

void MainWindow::on_pushButton_clicked()
{
    if(newSampleDialog->isHidden()){
        newSampleDialog->show();
    }
}


void MainWindow::on_comboBox_1_currentTextChanged(const QString &arg1)
{
    QStringList hostList0 = GetTableColEntries("Chemical", 0);
    QStringList hostList1 = GetTableColEntries("Chemical", 1);

    for(int i = 0; i < hostList0.size(); i ++ ){
        if( hostList0[i] == arg1) {
            ui->lineEdit_ChemFormula->setText(hostList1[i]);
            break;
        }
    }

}
