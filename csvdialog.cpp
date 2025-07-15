#include "csvdialog.h"
#include "ui_csvdialog.h"
#include "qtcsv/reader.h"
#include <QDebug>
#include <QTextCodec>

CSVDialog::CSVDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CSVDialog)
{
    ui->setupUi(this);
    connect(ui->cbEncoding, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &CSVDialog::update_table);
    connect(ui->cbCSVSeparator, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &CSVDialog::update_table);
    connect(ui->cbQuestionCol, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &CSVDialog::update_table);
    connect(ui->cbAnswerFirst, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &CSVDialog::update_table);
    connect(ui->cbAnswerLast, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &CSVDialog::update_table);
    connect(ui->cbCategory, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &CSVDialog::update_table);
    connect(ui->cbCorrect, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &CSVDialog::update_table);
    /*qDebug() << "=== Read csv-file and print it content to terminal ==";
     *
     * QList<QStringList> readData = QtCSV::Reader::readToList(fname);
     * for ( int i = 0; i < readData.size(); ++i )
     * {
     *  qDebug() << readData.at(i).join("|");
     * }*/
}

CSVDialog::~CSVDialog()
{
    delete ui;
}


QString CSVDialog::result()
{
    const QString      separator     = QString(ui->cbCSVSeparator->currentText() );
    const QString      textDelimiter = QString("\"");
    QTextCodec         *codec        = QTextCodec::codecForName(ui->cbEncoding->currentText().toLocal8Bit() );

    QList<QStringList> readData = QtCSV::Reader::readToList(path, separator, textDelimiter, codec);

    if (readData.isEmpty() )
    {
        return(QString() );
    }


    QString text = "";


    for (int i = ui->firstQuestion->value() - 1; i < readData.count(); i++)
    {
        // theme
        if (ui->cbCategory->currentIndex() > 0 && ui->cbCategory->currentIndex() <= readData.at(i).count() )
        {
            text += tr("\n@%1").arg(readData.at(i).at(ui->cbCategory->currentIndex() - 1) );
        }

        // question
        if (ui->cbQuestionCol->currentIndex() > 0 && ui->cbQuestionCol->currentIndex() <= readData.at(i).count() )
        {
            text += tr("\n?%1").arg(readData.at(i).at(ui->cbQuestionCol->currentIndex() - 1) );
        }

        //answers
        if (
            (ui->cbAnswerFirst->currentIndex() > 0)
            && (ui->cbAnswerFirst->currentIndex() < ui->cbAnswerLast->currentIndex() )
            && (ui->cbAnswerLast->currentIndex() <= readData.at(i).count() )
            && (ui->cbCorrect->currentIndex() > 0) && (ui->cbCorrect->currentIndex() <= readData.at(i).count() ) )
        {
            int correct = readData.at(i).at(ui->cbCorrect->currentIndex() - 1).toInt() - 1;


            for (int j = 0; j <= ui->cbAnswerLast->currentIndex() - ui->cbAnswerFirst->currentIndex(); j++)
            {
                if (j == correct)
                {
                    text += tr("\n+%1").arg(readData.at(i).at(ui->cbAnswerFirst->currentIndex() + j - 1) );
                } else {
                    text += tr("\n-%1").arg(readData.at(i).at(ui->cbAnswerFirst->currentIndex() + j - 1) );
                }
            }
        } else if ( (ui->cbCorrect->currentIndex() > 0) && (ui->cbCorrect->currentIndex() <= readData.at(i).count() ) )
        {
            text += tr("\n+%1").arg(readData.at(i).at(ui->cbCorrect->currentIndex() - 1) );
        }
    }

    return(text);
} // CSVDialog::result


void CSVDialog::update_table()
{
    const QString      separator     = QString(ui->cbCSVSeparator->currentText() );
    const QString      textDelimiter = QString("\"");
    QTextCodec         *codec        = QTextCodec::codecForName(ui->cbEncoding->currentText().toLocal8Bit() );

    QList<QStringList> readData = QtCSV::Reader::readToList(path, separator, textDelimiter, codec);

    if (readData.isEmpty() )
    {
        ui->table->setRowCount(1);
        ui->table->setColumnCount(1);


        QTableWidgetItem *newItem = new QTableWidgetItem(tr("empty") );


        ui->table->setItem(0, 0, newItem);

        return;
    }

    ui->table->setRowCount(4);
    ui->table->setColumnCount(readData.at(0).count() );

    for (int i = 0; i < 4; i++)
    {
        if (i > readData.count() )
        {
            ui->table->setRowCount(i);
            break;
        }

        for (int j = 0; j < readData.at(i).count(); j++)
        {
            QTableWidgetItem *newItem = new QTableWidgetItem(readData.at(i).at(j) );


            ui->table->setItem(i, j, newItem);
        }
    }


    QStringList cols;


    for (int j = 0; j <= readData.at(0).count(); j++)
    {
        cols << tr("%1").arg(j);
    }

    if (readData.at(0).count() > ui->cbQuestionCol->count() )
    {
        ui->cbQuestionCol->clear();
        ui->cbQuestionCol->addItems(cols);
    }

    if (readData.at(0).count() > ui->cbAnswerFirst->count() )
    {
        ui->cbAnswerFirst->clear();
        ui->cbAnswerFirst->addItems(cols);
    }

    if (readData.at(0).count() > ui->cbAnswerLast->count() )
    {
        ui->cbAnswerLast->clear();
        ui->cbAnswerLast->addItems(cols);
    }

    if (readData.at(0).count() > ui->cbCategory->count() )
    {
        ui->cbCategory->clear();
        ui->cbCategory->addItems(cols);
    }

    if (readData.at(0).count() > ui->cbCorrect->count() )
    {
        ui->cbCorrect->clear();
        ui->cbCorrect->addItems(cols);
    }

    //    const QString& separator,
    //    const QString& textDelimiter,
    //    QTextCodec* codec
    //    for ( int i = 0; i < readData.size(); ++i )
    //    {
    //        qDebug() << readData.at(i).join("|");
    //    }
} // CSVDialog::update_table

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
