#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator  myappTranslator;

    QString      tr1 = a.applicationDirPath();

    myappTranslator.load("TestConvert_" + QLocale::system().name(), tr1);

    a.installTranslator(&myappTranslator);


    MainWindow w;


    w.show();

    return(a.exec() );
}
