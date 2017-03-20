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
    qDebug() << "database open? " << db.open();
    QStringList tableList = db.tables();
    qDebug() << tableList;

    //===================== set up the sample-table
    sample = new QSqlRelationalTableModel(ui->sampleView);
    sample->setTable("Sample");
    sample->setEditStrategy(QSqlTableModel::OnManualSubmit);
    sample->select();

    //set relation, so that can choose directly on the table
    int chemicalIdx = sample->fieldIndex("Chemical");
    sample->setRelation(chemicalIdx, QSqlRelation("Chemical", "NAME", "NAME"));
    int hostIdx = sample->fieldIndex("Host");
    sample->setRelation(hostIdx, QSqlRelation("Host", "NAME", "NAME"));
    int solventIdx = sample->fieldIndex("Solvent");
    sample->setRelation(solventIdx, QSqlRelation("Solvent", "NAME", "NAME"));

    ui->sampleView->setModel(sample);
    ui->sampleView->resizeColumnsToContents();
    ui->sampleView->setItemDelegate(new QSqlRelationalDelegate(ui->sampleView));
    ui->sampleView->setItemDelegateForColumn(6, new DateFormatDelegate() );
    //ui->sampleView->setColumnHidden(sample->fieldIndex("ID"), true);
    ui->sampleView->setSelectionMode(QAbstractItemView::SingleSelection);

    //connect(ui->pushButton_sumbitSample, SIGNAL(clicked()), this, SLOT(submit()));

    //====================== set up the data-table
    data = new QSqlTableModel(this);
    data->setTable(tableList[0]);
    data->setEditStrategy(QSqlTableModel::OnManualSubmit);
    data->select();
    ui->dataView->setModel(data);
    ui->dataView->resizeColumnsToContents();

    ui->dataView->setItemDelegateForColumn(2, new DateFormatDelegate());

    //====================== Other things
    editor = NULL;

    ShowTable(tableList[4]);
    updateChemicalCombox(tableList[1]);

    //qDebug() << sample->fieldIndex("Name");

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

void MainWindow::updateChemicalCombox(QString tableName)
{
    QStringList hostList = GetTableColEntries(tableName, 1);
    ui->comboBox_chemical->clear();
    ui->comboBox_chemical->addItems(hostList);
}

void MainWindow::on_pushButton_editChemical_clicked()
{
    editor = new TableEditor("Chemical");
    disconnect(editor);
    connect(editor, SIGNAL(closed(QString)), this, SLOT(updateChemicalCombox(QString)));
    editor->show();
}

void MainWindow::on_comboBox_chemical_currentTextChanged(const QString &arg1)
{
    QStringList hostList0 = GetTableColEntries("Chemical", 1);
    QStringList hostList1 = GetTableColEntries("Chemical", 2);

    for(int i = 0; i < hostList0.size(); i ++ ){
        if( hostList0[i] == arg1) {
            ui->lineEdit_ChemFormula->setText(hostList1[i]);
            break;
        }
    }

    //TODO select sample
    QString filter = "Chemical='" + arg1 + "'";
    qDebug() << filter  ;
    sample->setFilter(filter);

}

void MainWindow::on_pushButton_selectSample_clicked()
{
    //TODO select data
    QModelIndex current = ui->sampleView->selectionModel()->currentIndex(); // the "current" item
    QString sampleName = sample->index(current.row(), 1).data().toString();

    QString filter = "Sample='" + sampleName + "'";
    qDebug() << filter;
    data->setFilter(filter);

}

void MainWindow::on_pushButton_sumbitSample_clicked()
{
    //sample->database().transaction();
    if (sample->submitAll()) {
    //    sample->database().commit();
        statusBar()->showMessage("Sample Database wriiten.");
    } else {
        sample->database().rollback();
        QMessageBox::warning(this, tr("Cached Table"),
                             tr("The database reported an error: %1")
                             .arg(sample->lastError().text()));
    }
}

void MainWindow::on_pushButton_addSampleEntry_clicked()
{
    sample->insertRow(sample->rowCount());
    ui->sampleView->scrollToBottom();
}

void MainWindow::on_pushButton_deleteSampleEntry_clicked()
{
    QModelIndex current = ui->sampleView->selectionModel()->currentIndex(); // the "current" item
    sample->removeRow(current.row());

    QString msg;
    msg.sprintf("Deleted Row #%d.", current.row()+1);
    statusBar()->showMessage(msg);
}

void MainWindow::on_pushButton_revertSample_clicked()
{
    sample->revertAll();
    sample->submitAll();

    statusBar()->showMessage("revert add/delete.");
}

void MainWindow::on_pushButton_submitData_clicked()
{
    //sample->database().transaction();
    if (data->submitAll()){
        statusBar()->showMessage("Data Database wriiten.");
    } else {
        data->database().rollback();
        QMessageBox::warning(this, tr("Cached Table"),
                             tr("The database reported an error: %1")
                             .arg(data->lastError().text()));
    }
}


void MainWindow::on_pushButton_addDataEntry_clicked()
{
    data->insertRow(data->rowCount());
    ui->dataView->scrollToBottom();
}

void MainWindow::on_pushButton_deleteDataEntry_clicked()
{
    QModelIndex current = ui->dataView->selectionModel()->currentIndex();
    data->removeRow(current.row());

    QString msg;
    msg.sprintf("Deleted Row #%d.", current.row()+1);
    statusBar()->showMessage(msg);
}

void MainWindow::on_pushButton_revertData_clicked()
{
    data->revertAll();
    sample->submitAll();

    statusBar()->showMessage("revert add/delete.");
}

void MainWindow::on_pushButton_open_clicked()
{
    // open the analysis program
    //QProcess process;
    //QString file = QDir::homepath + "file.exe";
    //process.start(file);
}
