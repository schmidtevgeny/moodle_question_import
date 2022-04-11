# требования по версии cmake
cmake_minimum_required(VERSION 3.20)

# переменная под название программы
set(TARGET_NAME TestConvert)
## разрешить поиск по переменным <имя>_ROOT
#cmake_policy(SET CMP0074 NEW)
# название проекта
project(${TARGET_NAME} VERSION "3.5.1")

#******************************************************************************
# поиск пакетов Qt
find_package(Qt5 REQUIRED
        Gui
        Widgets
        Core
        PrintSupport
        LinguistTools
        UiPlugin
        )

#SET(QT_USE_QTMAIN TRUE)
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
set(CMAKE_AUTORCC ON)
# собираемая программа - цель #2
add_executable(${TARGET_NAME} WIN32 ${UIC_SOURCES} ${RCC_SOURCES})
#-----------------------------------------------
target_sources(${TARGET_NAME} PRIVATE
        images/icon.qrc
        dialogreplace.cpp
        dialogreplace.h
        main.cpp
        mainwindow.cpp
        mainwindow.h
        myhighlighter.cpp
        myhighlighter.h
        highlighterdialog.cpp
        highlighterdialog.h
        textdialog.h
        textdialog.cpp
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
file(GLOB TRANSLATION_QM_FILES ${CMAKE_CURRENT_LIST_DIR}/*.qm)
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

install(TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION bin
        COMPONENT core)

install(DIRECTORY doc/
        DESTINATION "bin/doc"
        COMPONENT doc)

install(FILES ${TRANSLATION_QM_FILES}
        DESTINATION bin
        COMPONENT translate)

if (WIN32)
    install(IMPORTED_RUNTIME_ARTIFACTS
            Qt5::Widgets
            Qt5::Core
            Qt5::Gui
            Qt5::PrintSupport
            RUNTIME DESTINATION bin
            COMPONENT qtlib)
    set(QT_WINDOWS_QT_ROOT ${Qt5_DIR}/../../..)
    install(DIRECTORY ${QT_WINDOWS_QT_ROOT}/plugins/platforms/
            DESTINATION "bin/platforms"
            COMPONENT qtlib)
    install(DIRECTORY ${QT_WINDOWS_QT_ROOT}/plugins/styles/
            DESTINATION "bin/styles"
            COMPONENT qtlib)
    install(DIRECTORY ${QT_WINDOWS_QT_ROOT}/plugins/imageformats/
            DESTINATION "bin/imageformats"
            COMPONENT qtlib)
    # DEPLOY MSVC RUNTIME

    if (MSVC)
        set(COMPILER_REDISTRIB "MSVC")
        include(InstallRequiredSystemLibraries)
        install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION bin
                COMPONENT redistrib)
    else ()
        if (EXISTS ${QT_WINDOWS_QT_ROOT}/bin/libgcc_s_dw2-1.dll)
            set(COMPILER_REDISTRIB "mingw")
            install(FILES ${QT_WINDOWS_QT_ROOT}/bin/libgcc_s_dw2-1.dll
                    DESTINATION bin
                    COMPONENT redistrib)
        else ()
            set(COMPILER_REDISTRIB "mingw64")
            install(FILES ${QT_WINDOWS_QT_ROOT}/bin/libgcc_s_seh-1.dll
                    DESTINATION bin
                    COMPONENT redistrib)
        endif ()
        install(FILES
                ${QT_WINDOWS_QT_ROOT}/bin/libstdc++-6.dll
                ${QT_WINDOWS_QT_ROOT}/bin/libwinpthread-1.dll
                DESTINATION bin
                COMPONENT redistrib)
    endif ()
endif ()

# Задать некоторые характеристики пакета:
set(CPACK_PACKAGE_NAME "TestConvert")
set(CPACK_PACKAGE_VENDOR "TSU")
set(CPACK_PACKAGE_CONTACT "https://i-institute.tsu.tula.ru/moodle")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://i-institute.tsu.tula.ru/moodle")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "i-institute@yandex.ru")
set(CPACK_PACKAGE_DESCRIPTION "Moodle question converter")
set(CPACK_PACKAGE_EXECUTABLES ${TARGET_NAME} "Конвертер тестов")
set(CPACK_CREATE_DESKTOP_LINKS ${TARGET_NAME} "Конвертер тестов")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
#set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
#set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
#set(CPACK_PACKAGE_DESCRIPTION "Rjydthnth b ytvyjuj ntrcnf")

# Задать используемый генератор пакетов:
if (WIN32)
    set(CPACK_GENERATOR IFW)
    #    set(CPACK_GENERATOR ZIP IFW NSIS)
else ()
    set(CPACK_GENERATOR DEB ZIP RPM IFW)
endif ()
# Включить стандартный модуль "CPack":
include(CPack)
cpack_add_component(core
        DISPLAY_NAME "Ядро"
        DESCRIPTION "Основной модуль программы. Установка обязательна"
        REQUIRED)
cpack_add_component(doc
        DISPLAY_NAME "Справка"
        DESCRIPTION "Инструкция по работе с программой и требования к оформлению входных файлов."
        )
cpack_add_component(translate
        DISPLAY_NAME "Локализация"
        DESCRIPTION "Файлы для поддержки русского интерфейса"
        )
if (WIN32)
    cpack_add_component(qtlib
            DISPLAY_NAME "Библиотеки Qt"
            DESCRIPTION "Библиотеки Qt ${Qt5Widgets_VERSION_STRING}"
            )
    cpack_add_component(redistrib
            DISPLAY_NAME "Дополнительные библиотеки"
            DESCRIPTION "Распространяемый пакет библиотек для ${COMPILER_REDISTRIB}"
            )
endif ()