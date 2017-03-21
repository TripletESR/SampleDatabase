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
    SetupSampleTableView();


    //====================== set up the data-table
    data = new QSqlRelationalTableModel(this);
    data->setTable("Data");
    data->setEditStrategy(QSqlTableModel::OnManualSubmit);
    data->select();
    ui->dataView->setModel(data);
    ui->dataView->resizeColumnsToContents();

    int sampleIdx = data->fieldIndex("Sample");
    data->setRelation(sampleIdx, QSqlRelation("Sample", "NAME", "NAME"));
    ui->dataView->setItemDelegate(new QSqlRelationalDelegate(ui->sampleView));
    ui->dataView->setItemDelegateForColumn(2, new DateFormatDelegate());
    ui->dataView->setItemDelegateForColumn(5, new OpenFileDelegate());
    ui->dataView->horizontalHeader()->model()->setHeaderData(1, Qt::Horizontal, "Sample");

    //====================== Other things
    editorChemical = NULL;
    editorHost = NULL;
    editorSolvent = NULL;

    ShowTable("Chemical");
    ShowTable("Sample");
    ShowTable("Data");

    updateChemicalCombox("Chemical");

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

    ui->sampleView->setModel(sample);
    ui->sampleView->resizeColumnsToContents();
    ui->sampleView->setItemDelegate(new QSqlRelationalDelegate(ui->sampleView));
    ui->sampleView->setItemDelegateForColumn(6, new DateFormatDelegate() );
    //ui->sampleView->setColumnHidden(sample->fieldIndex("ID"), true);
    ui->sampleView->setSelectionMode(QAbstractItemView::SingleSelection);
    //for some unknown reasons, the column header names are needed to rename;
    ui->sampleView->horizontalHeader()->model()->setHeaderData(2, Qt::Horizontal, "Chemical");
    ui->sampleView->horizontalHeader()->model()->setHeaderData(3, Qt::Horizontal, "Host");
    ui->sampleView->horizontalHeader()->model()->setHeaderData(4, Qt::Horizontal, "Solvent");

    //connect(ui->pushButton_sumbitSample, SIGNAL(clicked()), this, SLOT(submit()));
}


void MainWindow::updateChemicalCombox(QString tableName)
{
    QStringList hostList = GetTableColEntries(tableName, 1);
    ui->comboBox_chemical->clear();
    ui->comboBox_chemical->addItem("All");
    ui->comboBox_chemical->addItems(hostList);
}

void MainWindow::on_pushButton_editChemical_clicked()
{
    editorChemical = new TableEditor("Chemical");
    disconnect(editorChemical);
    connect(editorChemical, SIGNAL(closed(QString)), this, SLOT(updateChemicalCombox(QString)));
    connect(editorChemical, SIGNAL(closed(QString)), this, SLOT(SetupSampleTableView()));
    editorChemical->show();
}

void MainWindow::on_comboBox_chemical_currentTextChanged(const QString &arg1)
{
    if(arg1 == "All") {
        sample->setFilter("");
        data->setFilter("");
        ui->lineEdit_ChemFormula->setText("-----");
        ui->label_Picture->clear();
        return;
    }

    QStringList nameList = GetTableColEntries("Chemical", 1);
    QStringList formulaList = GetTableColEntries("Chemical", 2);
    QStringList picPathList = GetTableColEntries("Chemical", 3);

    for(int i = 0; i < nameList.size(); i ++ ){
        if( nameList[i] == arg1) {
            ui->lineEdit_ChemFormula->setText(formulaList[i]);
            QImage image(picPathList[i]);
            ui->label_Picture->setPixmap(QPixmap::fromImage(image));
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
    int row = data->rowCount();
    data->insertRow(row);
    ui->dataView->scrollToBottom();

    //set default data
    QDate date;
    data->setData(sample->index(row, 2), date.currentDate().toString("yyyy-MM-dd"));
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

void MainWindow::on_pushButton_editHost_clicked()
{
    editorHost = new TableEditor("Host");
    disconnect(editorHost);
    connect(editorHost, SIGNAL(closed(QString)), this, SLOT(SetupSampleTableView()));
    editorHost->show();
}

void MainWindow::on_pushButton_editSolvent_clicked()
{
    editorSolvent = new TableEditor("Solvent");
    disconnect(editorSolvent);
    connect(editorSolvent, SIGNAL(closed(QString)), this, SLOT(SetupSampleTableView()));
    editorSolvent->show();
}
