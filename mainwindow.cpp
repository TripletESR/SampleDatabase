#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(1500,500);
    int configFileFlag  = loadConfigurationFile();

    if(configFileFlag == 1) {
        ErrorAndClose("The configuration file not exist.\n"
                      "please check the ProgramConfiguration.ini exist on Desktop.");
    }else if(configFileFlag == 2){
        ErrorAndClose("The configuration file fail to open.");
    }else if(configFileFlag == 3){
        ErrorAndClose("Some items are missing in configuration file.");
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    if( QFile::exists(DB_PATH) ){
        qDebug() << "database exist : " << DB_PATH ;
    }else{
        ErrorAndClose("No database : " + DB_PATH) ;
    }
    db.setDatabaseName(DB_PATH);
    db.open();
    if( !db.isOpen()){
        ErrorAndClose("Database open Error : " + DB_PATH );
    }else{
        statusBar()->showMessage("Database openned. | " + DB_PATH);
        QString fileInfo = DB_PATH + " | ";
        QFileInfo info(DB_PATH);
        double size = info.size()/1024. ; // in kb;
        fileInfo += "Size : " +  QString::number(size) + " kb | ";
        QDateTime date = info.lastModified();
        fileInfo += "Last modified date : " + date.toString("yyyy-MM-dd, HH:mm:ss") + " | ";
        QDateTime today;
        double dayDiff = date.secsTo(today.currentDateTime()) / 24./ 2600.;
        fileInfo += "Last modified from today : " + QString::number(dayDiff) + " day(s) |";
        ui->lineEdit_dbName->setText( fileInfo );
    }

    QStringList tableList = db.tables();
    qDebug() << tableList;

    //===================== set up the sample-table
    sample = new QSqlRelationalTableModel(ui->sampleView);
    SetupSampleTableView();

    //===================== set up the sample-table
    data = new QSqlRelationalTableModel(ui->dataView);
    SetupESRDataTableView();

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

void MainWindow::SaveTable(QString tableName, QString showName, QTextStream &stream)
{
    stream <<"================== " + showName +"\n";

    QSqlQuery query;

    query.exec("SELECT *FROM " + tableName);
    int col = query.record().count();

   for( int i = 0; i < col; i++){
       stream << query.record().fieldName(i);
       if( i < col-1 ) stream << ", ";
   }
   stream << "\n";

    while(query.next()){
        for( int i = 0; i < col; i++){
            stream << query.value(i).toString();
            if( i < col-1 )stream << ", ";
        }
        stream << "\n";
    }

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

void MainWindow::closeEvent(QCloseEvent *event)
{
    //event->ignore();
    QApplication::closeAllWindows();
    event->accept();
}

void MainWindow::ErrorAndClose(QString msg)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.exec();
    QTimer::singleShot(0, this, SLOT(close()));
}

void MainWindow::SetupSampleTableView()
{
    sample->clear();
    sample->setTable("Sample");
    sample->setEditStrategy(QSqlTableModel::OnManualSubmit);

    //set relation, so that can choose directly on the table
    int chemicalIdx = sample->fieldIndex("ChemicalID");
    int solventIdx = sample->fieldIndex("SolventID");
    int dateIdx = sample->fieldIndex("Date");
    int picPathIdx = sample->fieldIndex("PicPath");
    int spectPathIdx = sample->fieldIndex("SpectrumPath");

    //for some unknown reasons, the column header names are needed to rename;
    sample->setHeaderData(chemicalIdx, Qt::Horizontal, "Pol. Agent");
    sample->setHeaderData(solventIdx, Qt::Horizontal, "Solvent");

    sample->setRelation(chemicalIdx, QSqlRelation("Chemical", "ID", "NAME"));
    sample->setRelation(solventIdx, QSqlRelation("Solvent", "ID", "NAME"));
    sample->select();

    ui->sampleView->setModel(sample);
    ui->sampleView->resizeColumnsToContents();
    ui->sampleView->setItemDelegate(new QSqlRelationalDelegate(ui->sampleView));
    ui->sampleView->setItemDelegateForColumn(dateIdx, new DateFormatDelegate() );
    ui->sampleView->setItemDelegateForColumn(picPathIdx, new OpenFileDelegate() );
    ui->sampleView->setItemDelegateForColumn(spectPathIdx, new OpenFileDelegate() );
    ui->sampleView->setSelectionMode(QAbstractItemView::SingleSelection);
    //ui->sampleView->setColumnHidden(sample->fieldIndex("ID"), true);

    ui->sampleView->setColumnWidth(sample->fieldIndex("ID"), 30);
    ui->sampleView->setColumnWidth(chemicalIdx, 100);
    ui->sampleView->setColumnWidth(solventIdx, 100);
    ui->sampleView->setColumnWidth(dateIdx, 100);

    //connect(ui->pushButton_sumbitSample, SIGNAL(clicked()), this, SLOT(submit()));

}

void MainWindow::SetupESRDataTableView()
{
    data->clear();
    data->setTable("Data");
    data->setEditStrategy(QSqlTableModel::OnManualSubmit);

    int sampleIdx = data->fieldIndex("SampleID");
    int dateIdx = data->fieldIndex("Date");
    int laserIdx = data->fieldIndex("LaserID");
    int pathIdx = data->fieldIndex("PATH");
    int repIdx = data->fieldIndex("repetition");
    int accIdx = data->fieldIndex("Average");
    int pointIdx = data->fieldIndex("DataPoint");
    int tempIdx = data->fieldIndex("Temperature");
    int timeRangeIdx = data->fieldIndex("TimeRange");

    data->setHeaderData(sampleIdx, Qt::Horizontal, "Sample");
    data->setHeaderData(laserIdx, Qt::Horizontal, "Laser");
    data->setHeaderData(repIdx, Qt::Horizontal, "Trig.\nRate [Hz]");
    data->setHeaderData(accIdx, Qt::Horizontal, "Accum.");
    data->setHeaderData(pointIdx, Qt::Horizontal, "Data\nPoint");
    data->setHeaderData(tempIdx, Qt::Horizontal, "Temp.\n[K]");
    data->setHeaderData(timeRangeIdx, Qt::Horizontal, "Time\nRange[us]");

    data->setRelation(sampleIdx, QSqlRelation("Sample", "ID", "NAME"));
    data->setRelation(laserIdx, QSqlRelation("Laser", "ID", "Name"));
    data->select();

    ui->dataView->setModel(data);
    ui->dataView->resizeColumnsToContents();
    ui->dataView->setSelectionMode( QAbstractItemView::SingleSelection );
    ui->dataView->setItemDelegate(new QSqlRelationalDelegate(ui->dataView));
    ui->dataView->setItemDelegateForColumn(dateIdx, new DateFormatDelegate());
    ui->dataView->setItemDelegateForColumn(pathIdx, new OpenFileDelegate(2));

    ui->dataView->setColumnWidth(sampleIdx, 150);
    ui->dataView->setColumnWidth(dateIdx, 100);
    ui->dataView->setColumnWidth(laserIdx, 120);

}

void MainWindow::SetupNMRDataTableView()
{
    data->clear();
    data->setTable("NMRData");
    data->setEditStrategy(QSqlTableModel::OnManualSubmit);

    int sampleIdx = data->fieldIndex("SampleID");
    int dateIdx = data->fieldIndex("Date");
    int magIdx = data->fieldIndex("MagField");
    int laserIdx = data->fieldIndex("LaserID");
    int pathIdx = data->fieldIndex("Path");
    int repIdx = data->fieldIndex("repetition");
    int accIdx = data->fieldIndex("Accumulation");
    int pointIdx = data->fieldIndex("DataPoint");
    int tempIdx = data->fieldIndex("Temperature");
    int timeRangeIdx = data->fieldIndex("TimeRange");
    int commentIdx = data->fieldIndex("Comment");

    data->setHeaderData(sampleIdx, Qt::Horizontal, "Sample");
    data->setHeaderData(laserIdx, Qt::Horizontal, "Laser");
    data->setHeaderData(magIdx, Qt::Horizontal, "Mag.\nField [mT]");
    data->setHeaderData(repIdx, Qt::Horizontal, "Trig.\nRate [Hz]");
    data->setHeaderData(accIdx, Qt::Horizontal, "Accum.");
    data->setHeaderData(pointIdx, Qt::Horizontal, "Data\nPoint");
    data->setHeaderData(tempIdx, Qt::Horizontal, "Temp.\n[K]");
    data->setHeaderData(timeRangeIdx, Qt::Horizontal, "Time\nRange[us]");

    data->setRelation(sampleIdx, QSqlRelation("Sample", "ID", "NAME"));
    data->setRelation(laserIdx, QSqlRelation("Laser", "ID", "Name"));
    data->select(); // kind of finish the setting;

    QAbstractItemDelegate * normalDelegate = ui->dataView->itemDelegateForColumn(sampleIdx);

    ui->dataView->setModel(data);
    ui->dataView->resizeColumnsToContents();
    ui->dataView->setSelectionMode( QAbstractItemView::SingleSelection );
    ui->dataView->setItemDelegate(new QSqlRelationalDelegate(ui->dataView));
    ui->dataView->setItemDelegateForColumn(commentIdx, normalDelegate);
    ui->dataView->setItemDelegateForColumn(dateIdx, new DateFormatDelegate());
    ui->dataView->setItemDelegateForColumn(pathIdx, new OpenFileDelegate(2));

    ui->dataView->setColumnWidth(sampleIdx, 150);
    ui->dataView->setColumnWidth(dateIdx, 100);
    ui->dataView->setColumnWidth(laserIdx, 120);

    //data->submit();
    //data->submitAll();
}

int MainWindow::loadConfigurationFile()
{
    QString path = DESKTOP_PATH + "/ProgramsConfiguration.ini";
    if( QFile::exists(path) ){
        qDebug() << "Configuration file found :" << path;
    }else{
        qDebug() << "Configuration not found. | " << path;
        return 1;
    }

    QFile configFile(path);
    configFile.open(QIODevice::ReadOnly);
    if( configFile.isOpen() ){
        qDebug("Configuration file openned.");
    }else{
        qDebug("Configuration file fail to open.");
        return 2;
    }

    QTextStream stream(&configFile);
    QString line;
    QStringList lineList;

    int itemCount = 0;

    while(stream.readLineInto(&line) ){
        if( line.left(1) == "#" ) continue;
        lineList = line.split(" ");
        //qDebug() << lineList[0] << ", " << lineList[lineList.size()-1];
        if( lineList[0] == "DATA_PATH") {
            DATA_PATH = lineList[lineList.size()-1];
            itemCount ++;
        }
        if( lineList[0] == "DB_PATH") {
            DB_PATH = lineList[lineList.size()-1];
            itemCount ++;
        }
        if( lineList[0] == "HALL_PATH") {
            HALL_PATH = lineList[lineList.size()-1];
            itemCount ++;
        }
        if( lineList[0] == "LOG_PATH") {
            LOG_PATH = lineList[lineList.size()-1];
            itemCount ++;
        }
        if( lineList[0] == "ChemicalPicture_PATH") {
            CHEMICAL_PIC_PATH = lineList[lineList.size()-1];
            itemCount ++;
        }
        if( lineList[0] == "SamplePicture_PATH") {
            SAMPLE_PIC_PATH = lineList[lineList.size()-1];
            itemCount ++;
        }
    }

    if( itemCount != 6){
        return 3;
    }

    return 0;
}

void MainWindow::on_pushButton_editChemical_clicked()
{
    editorChemical = new TableEditor("Chemical");
    editorChemical->resize(600,200);
    editorChemical->setWindowFlags(Qt::WindowStaysOnTopHint);
    disconnect(editorChemical);
    connect(editorChemical, SIGNAL(closed(QString)), this, SLOT(SetupSampleTableView()));
    editorChemical->show();
}

void MainWindow::on_pushButton_editSolvent_clicked()
{
    editorSolvent = new TableEditor("Solvent");
    editorSolvent->resize(400,200);
    editorSolvent->setWindowFlags(Qt::WindowStaysOnTopHint);
    disconnect(editorSolvent);
    connect(editorSolvent, SIGNAL(closed(QString)), this, SLOT(SetupSampleTableView()));
    editorSolvent->show();
}

void MainWindow::on_pushButton_editLaser_clicked()
{
    editorLaser = new TableEditor("Laser");
    editorLaser->resize(400,200);
    editorLaser->setWindowFlags(Qt::WindowStaysOnTopHint);
    disconnect(editorLaser);
    connect(editorLaser, SIGNAL(closed(QString)), this, SLOT(SetupESRDataTableView()));
    editorLaser->show();
}

void MainWindow::on_pushButton_sumbitSample_clicked()
{
    if (sample->submitAll()) {
        statusBar()->showMessage("Sample Database wriiten.");
        SetupESRDataTableView();
    } else {
        sample->database().rollback();
        QMessageBox::warning(this, tr("Cached Table"),
                             tr("The database reported an error: %1")
                             .arg(sample->lastError().text()));
    }

    SetupESRDataTableView();

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
    int dateIdx = sample->fieldIndex("Date");
    sample->setData(sample->index(row, dateIdx), date.currentDate().toString("yyyy-MM-dd"));
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


void MainWindow::on_pushButton_addDataEntry_clicked()
{
    int row = data->rowCount();
    data->insertRow(row);
    ui->dataView->scrollToBottom();

    //set default data
    QDate date;
    int dateIdx = data->fieldIndex("Date");
    data->setData(data->index(row, dateIdx), date.currentDate().toString("yyyy-MM-dd"));
}

void MainWindow::on_pushButton_deleteDataEntry_clicked()
{
    QModelIndex current = ui->dataView->selectionModel()->currentIndex(); // the "current" item
    data->removeRow(current.row());

    QString msg;
    msg.sprintf("Deleted Row #%d.", current.row()+1);
    statusBar()->showMessage(msg);
}

void MainWindow::on_pushButton_revertData_clicked()
{
    data->revertAll();
    data->submitAll();

    statusBar()->showMessage("revert add/delete.");
}

void MainWindow::on_pushButton_submitData_clicked()
{
    //sample->database().transaction();
    if (data->submitAll()) {
    //    sample->database().commit();
        statusBar()->showMessage("Sample Database wriiten.");
    } else {
        data->database().rollback();
        QMessageBox::warning(this, tr("Cached Table"),
                             tr("The database reported an error: %1")
                             .arg(data->lastError().text()));
    }
}

void MainWindow::on_actionOutput_tables_triggered()
{

    QString filePath = QFileDialog::getSaveFileName(this,
                                                  "Save File as CSV or TXT",
                                                  DESKTOP_PATH,
                                                  "TEXT (*.txt);; CSV (*.csv)");

    qDebug() << filePath;

    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox msgBox;
        msgBox.setText("Fail to open file:\n" + filePath);
        msgBox.exec();
        return;
    }

    QTextStream stream(&file);
    QString text;

    QDateTime date;
    text = "#### Saving datebase tables into txt or cvs file. \n";
    stream << text;
    text = "#### Date : " + date.currentDateTime().toString("yyyy-MM-dd, HH:mm:ss") + "\n";
    stream << text;
    text = "#### DB : " + DB_PATH + "\n";
    stream << text;

    SaveTable("Chemical", "Pol. Agent", stream);
    SaveTable("Solvent", "Host/Solvent", stream);
    SaveTable("Laser", "Laser", stream);
    SaveTable("Sample", "Sample", stream);
    SaveTable("Data", "Data", stream);

    file.close();

}

void MainWindow::on_radioButton_ESRData_clicked()
{
    SetupESRDataTableView();
}

void MainWindow::on_radioButton_NMRData_clicked()
{
    SetupNMRDataTableView();
}
