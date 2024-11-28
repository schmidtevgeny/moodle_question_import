#------------------------------------------------ -
#
#Project created by QtCreator 2015 - 10 - 29T10 : 49 : 03
#
#------------------------------------------------ -

QT += core gui widgets

TARGET = TestConvert

TEMPLATE = app

TRANSLATIONS = TestConvert_ru.ts

SOURCES += main.cpp mainwindow.cpp dialogreplace.cpp myhighlighter.cpp highlighterdialog.cpp \
    dialogconfig.cpp \
    textdialog.cpp

HEADERS += mainwindow.h dialogreplace.h myhighlighter.h highlighterdialog.h \
    dialogconfig.h \
    textdialog.h

FORMS += mainwindow.ui dialogreplace.ui highlighterdialog.ui \
    dialogconfig.ui

RESOURCES += images/icon.qrc

DISTFILES += \
    .gitignore \
    CMakeLists.txt \
    LICENSE \
    TestConvert_ru.qm \
    fragments \
    install.iss \
    postlink.bat \
    readme-ru.md \
    readme.md \
    uncrustify.cfg
