#include "newsampledialog.h"
#include "ui_newsampledialog.h"

NewSampleDialog::NewSampleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSampleDialog)
{
    ui->setupUi(this);
}

NewSampleDialog::~NewSampleDialog()
{
    delete ui;
}
