#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
#
#Project created by QtCreator 2015 - 10 - 29T10 : 49 : 03
#
#-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -

QT += core gui widgets

    TARGET = TestConvert

        TEMPLATE = app

            TRANSLATIONS = TestConvert_ru.ts

                               SOURCES +=
    main.cpp mainwindow.cpp dialogreplace.cpp myhighlighter.cpp highlighterdialog.cpp

        HEADERS += mainwindow.h dialogreplace.h myhighlighter.h highlighterdialog.h

                       FORMS += mainwindow.ui dialogreplace.ui highlighterdialog.ui

#QMAKE_POST_LINK = E :\TestConvert\postlink.batL

                                    RESOURCES += images / icon.qrc
