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

QString CSVDialog::result() {
    return QString();
}

//
//            QFile f(fname);
//
//
//            f.open(QIODevice::ReadOnly);
//
//
//            QTextStream in(&f);
//
//
//            in.setCodec("utf-8");
//
//
//            QString     s;
//            QStringList line;
//
//            QString     s_all = in.readAll();
//
//
//            for (auto s_line : s_all.split("\n") )
//            {
//                line = s_line.split(";");
//
//                if (line.count() < 15)
//                {
//                    continue;
//                }
//
//                s += "\n@" + line[13];
//                s += "\n?" + line[1];
//
//
//                int correct = line[14].toInt();
//
//
//                for (int i = 1; i < 11; i++)
//                {
//                    if (line[1 + i].isEmpty() )
//                    {
//                        continue;
//                    }
//
//                    if (i == correct)
//                    {
//                        s += "\n+";
//                    } else {
//                        s += "\n-";
//                    }
//
//                    s += line[i + 1];
//                }
//            }
