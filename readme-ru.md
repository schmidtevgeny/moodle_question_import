Конвертер тестов СДО moodle3
=============================

Программа предназначена для быстрого создания файлов формата Moodle XML
для импорта тестовых вопросов.

Поддерживаются следующие типы вопросов
---------------------------------------


Правила оформления вопросов
----------------------------

\# Section

\@ Subsection

? Question choice or multichoice

\* correct

incorrect

? Question numerical or shortanswer

\* answer

? Question matching

\* option1->value1
\* option2->value2

TODO: clozed question

$ Clozed question intro

? Subquestion choice or multichoice

\* correct

incorrect

? Subquestion numerical or shortanswer

\* answer

? Subquestion matching

\* option1->value1
\* option2->value2
 

Состав проекта
---------------

- dialogreplace.* - диалоговое окно замены
- mainwindow.* - основной исполняемый модуль, главное окно
- main.cpp - точка входа
- myhighlighter.* - подсветка текста
- install.iss - скрипт установки
- postlink.bat - сборка установщика
- TestConvert.pro - файл проекта qmake
- CMakeLists.txt - файл проектс cmake
- TestConvert_ru.ts - файл русской локализации