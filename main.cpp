#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator  myappTranslator;


    myappTranslator.load("TestConvert_" + QLocale::system().name() );

    a.installTranslator(&myappTranslator);


    MainWindow w;


    w.show();

    return(a.exec() );
}
