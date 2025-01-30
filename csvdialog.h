#ifndef CSVDIALOG_H
#define CSVDIALOG_H

#include <QDialog>

namespace Ui {
class CSVDialog;
}

class CSVDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CSVDialog(QWidget *parent = nullptr);
    ~CSVDialog();

private:
    Ui::CSVDialog *ui;
};

#endif // CSVDIALOG_H
