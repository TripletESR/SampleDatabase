#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase("QSQLITE");
    if( QFile::exists(DB_PATH) ){
        qDebug() << "database exist : " << DB_PATH ;
    }else{
        qDebug() << "No database : " << DB_PATH ;
        return;
    }
    db.setDatabaseName(DB_PATH);
    db.open();
    if( !db.isOpen()){
        qDebug() << "Database open Error : " + DB_PATH ;
        return;
    }else{
        statusBar()->showMessage("Database openned. | " + DB_PATH);
    }

    QStringList tableList = db.tables();
    qDebug() << tableList;

    //===================== set up the sample-table
    sample = new QSqlRelationalTableModel(ui->sampleView);
    SetupSampleTableView();

    //====================== Other things
    editorChemical = NULL;
    editorSolvent = NULL;
    editorLaser = NULL;

    //ShowTable("Chemical");
    //ShowTable("Sample");
    //ShowTable("Data");

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

void MainWindow::SetupSampleTableView()
{
    sample->clear();
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
    int dateIdx = sample->fieldIndex("Date");
    int pathIdx = sample->fieldIndex("SpectrumPath");

    ui->sampleView->setModel(sample);
    ui->sampleView->resizeColumnsToContents();
    ui->sampleView->setItemDelegate(new QSqlRelationalDelegate(ui->sampleView));
    ui->sampleView->setItemDelegateForColumn(dateIdx, new DateFormatDelegate() );
    ui->sampleView->setItemDelegateForColumn(pathIdx, new OpenFileDelegate() );
    ui->sampleView->setSelectionMode(QAbstractItemView::SingleSelection);
    //ui->sampleView->setColumnHidden(sample->fieldIndex("ID"), true);

    //for some unknown reasons, the column header names are needed to rename;
    sample->setHeaderData(chemicalIdx, Qt::Horizontal, "Chemical");
    sample->setHeaderData(hostIdx, Qt::Horizontal, "Host");
    sample->setHeaderData(solventIdx, Qt::Horizontal, "Solvent");

    ui->sampleView->setColumnWidth(sample->fieldIndex("ID"), 30);
    ui->sampleView->setColumnWidth(chemicalIdx, 100);
    ui->sampleView->setColumnWidth(hostIdx, 100);
    ui->sampleView->setColumnWidth(solventIdx, 100);
    ui->sampleView->setColumnWidth(dateIdx, 100);

    //connect(ui->pushButton_sumbitSample, SIGNAL(clicked()), this, SLOT(submit()));

}

void MainWindow::on_pushButton_editChemical_clicked()
{
    editorChemical = new TableEditor("Chemical");
    disconnect(editorChemical);
    connect(editorChemical, SIGNAL(closed(QString)), this, SLOT(SetupSampleTableView()));
    editorChemical->show();
}

void MainWindow::on_pushButton_editSolvent_clicked()
{
    editorSolvent = new TableEditor("Solvent");
    disconnect(editorSolvent);
    connect(editorSolvent, SIGNAL(closed(QString)), this, SLOT(SetupSampleTableView()));
    editorSolvent->show();
}

void MainWindow::on_pushButton_editLaser_clicked()
{
    editorLaser = new TableEditor("Laser");
    editorLaser->show();
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
    int row = sample->rowCount();
    sample->insertRow(row);
    ui->sampleView->scrollToBottom();

    //set default data
    QString sampleName = "Sample-" + QString::number(row+1);
    sample->setData(sample->index(row,1), sampleName);
    QDate date;
    sample->setData(sample->index(row, 6), date.currentDate().toString("yyyy-MM-dd"));
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

