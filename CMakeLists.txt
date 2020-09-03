# требования по версии cmake
cmake_minimum_required(VERSION 3.10)

# переменная под название программы
set(TARGET_NAME TestConvert)
# разрешить поиск по переменным <имя>_ROOT
cmake_policy(SET CMP0074 NEW)
# название проекта
project(${TARGET_NAME})

#******************************************************************************
# поиск пакетов Qt
find_package(Qt5 REQUIRED
        Gui
        Widgets
        Core
        PrintSupport
        LinguistTools
        )

SET(QT_USE_QTMAIN TRUE)
# moc от qt
# список форм
qt5_wrap_ui(UIC_SOURCES
        dialogreplace.ui
        mainwindow.ui
        highlighterdialog.ui)
#ресурсы
qt5_add_resources(RCC_SOURCES
        images/icon.qrc)
# автовызов moc
set(CMAKE_AUTOMOC ON)
# собираемая программа - цель #2
add_executable(${TARGET_NAME} WIN32 ${UIC_SOURCES} ${RCC_SOURCES})
#-----------------------------------------------
target_sources(${TARGET_NAME} PRIVATE
        dialogreplace.cpp
        dialogreplace.h
        main.cpp
        mainwindow.cpp
        mainwindow.h
        myhighlighter.cpp
        myhighlighter.h
        highlighterdialog.cpp
        highlighterdialog.h
        )
# Включаем C++ 2014
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_14)
# пути поиска
target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

# пихаем в переменную файлы по шаблону поиска
file(GLOB TRANSLATION_FILES ${CMAKE_CURRENT_LIST_DIR}/*.ts)
file(GLOB FILES_TO_TRANSLATE
        ${CMAKE_CURRENT_LIST_DIR}/*.ui
        ${CMAKE_CURRENT_LIST_DIR}/*.cpp
        )
# список файлов для перевода
qt5_create_translation(TRANSLATION_MESSAGES ${FILES_TO_TRANSLATE} ${TRANSLATION_FILES})
# список файлов перевода
qt5_add_translation(TRANSLATION_QM ${TRANSLATION_FILES})
# добавляем цель 3 - файлы перевода создать
add_custom_target(translations_update DEPENDS ${TRANSLATION_MESSAGES})
# добавляем цель 4 - файлы перевода скомпилировать
add_custom_target(translations DEPENDS ${TRANSLATION_QM})
# дополнительные действия после сборки
# линковка с библиотеками
target_link_libraries(${TARGET_NAME}
        Qt5::Widgets
        Qt5::Core
        Qt5::Gui
        Qt5::PrintSupport
        )

