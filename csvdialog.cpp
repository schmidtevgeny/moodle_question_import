#include "csvdialog.h"
#include "ui_csvdialog.h"
#include "qtcsv/reader.h"
#include <QDebug>

CSVDialog::CSVDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CSVDialog)
{
    ui->setupUi(this);
    /*qDebug() << "=== Read csv-file and print it content to terminal ==";

    QList<QStringList> readData = QtCSV::Reader::readToList(fname);
    for ( int i = 0; i < readData.size(); ++i )
    {
        qDebug() << readData.at(i).join("|");
    }*/
}

CSVDialog::~CSVDialog()
{
    delete ui;
}
