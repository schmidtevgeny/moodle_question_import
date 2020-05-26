#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
#
#Project created by QtCreator 2015 - 10 - 29T10 : 49 : 03
#
#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -

QT += core gui widgets

TARGET = TestConvert

TEMPLATE = app

TRANSLATIONS = TestConvert.ts

SOURCES += main.cpp mainwindow.cpp dialogreplace.cpp myhighlighter.cpp

HEADERS += mainwindow.h dialogreplace.h myhighlighter.h

FORMS += mainwindow.ui dialogreplace.ui

#QMAKE_POST_LINK = E:\TestConvert\postlink.bat

RESOURCES += images/icon.qrc
