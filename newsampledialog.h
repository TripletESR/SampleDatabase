#ifndef NEWSAMPLEDIALOG_H
#define NEWSAMPLEDIALOG_H

#include <QDialog>

namespace Ui {
class NewSampleDialog;
}

class NewSampleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewSampleDialog(QWidget *parent = 0);
    ~NewSampleDialog();

private:
    Ui::NewSampleDialog *ui;
};

#endif // NEWSAMPLEDIALOG_H
