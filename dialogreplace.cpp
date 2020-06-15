#include "dialogreplace.h"
#include "ui_dialogreplace.h"

DialogReplace::DialogReplace(QWidget * parent) : QDialog(parent), ui(new Ui::DialogReplace) {
    ui->setupUi(this);
    // глобальные замены
    // global substitutions
    ui->repl_questnum->setToolTip(
        "<html><head/><body><p>Заменяет в начале строки</p><p>"
        "<i>пробелы</i>|<i>знак вопроса</i>|<i>числа</i>|<i>пробелы</i>|<i>точка</i>|<i>пробелы</i></p>"
        "<p>на <i>знак вопроса</i></p></body></html>");
    ui->repl_numdot->setToolTip(
        "<html><head/><body><p>Заменяет в начале строки</p><p>"
        "<i>пробелы</i>|<i>числа</i>|<i>пробелы</i>|<i>точка</i>|<i>пробелы</i></p>"
        "<p>на <i>знак вопроса</i></p></body></html>");
    ui->repl_num->setToolTip(
        "<html><head/><body><p>Заменяет в начале строки</p><p>"
        "<i>пробелы</i>|<i>числа</i>|<i>пробелы</i></p>"
        "<p>на <i>знак вопроса</i></p></body></html>");
    ui->repl_numsk->setToolTip(
        "<html><head/><body><p>Заменяет в начале строки</p><p>"
        "<i>пробелы</i>|<i>числа</i>|<i>пробелы</i>|<i>круглая скобка</i>|<i>пробелы</i></p>"
        "<p>на <i>знак вопроса</i></p></body></html>");
    ui->del_alpha->setToolTip(
        "<html><head/><body><p>Заменяет в начале строки</p><p>"
        "<i>пробелы</i>|<i>звездочка</i>|<i>буква</i>|<i>пробелы</i>|<i>круглая скобка</i>|<i>пробелы</i></p>"
        "<p>на <i>звездочку</i> и удаляет</p>"
        "<p><i>пробелы</i>|<i>буква</i>|<i>пробелы</i>|<i>круглая скобка</i>|<i>пробелы</i></p></body></html>");
    ui->del_alphadot->setToolTip(
        "<html><head/><body><p>Заменяет в начале строки</p><p>"
        "<i>пробелы</i>|<i>звездочка</i>|<i>буква</i>|<i>пробелы</i>|<i>точка</i>|<i>пробелы</i></p>"
        "<p>на <i>звездочку</i> и удаляет</p>"
        "<p><i>пробелы</i>|<i>буква</i>|<i>пробелы</i>|<i>точка</i>|<i>пробелы</i></p></body></html>");
    ui->del_num->setToolTip(
        "<html><head/><body><p><font color=red>Опасность для чисел</font></p>"
        "<p>Заменяет в начале строки</p><p>"
        "<i>пробелы</i>|<i>звездочка</i>|<i>число</i>|<i>пробелы</i>|<i>точка</i>|<i>пробелы</i></p>"
        "<p>на <i>звездочку</i> и удаляет</p>"
        "<p><i>пробелы</i>|<i>число</i>|<i>пробелы</i>|<i>точка</i>|<i>пробелы</i></p></body></html>");
    ui->del_numsk->setToolTip(
        "<html><head/><body><p><font color=red>Опасность для чисел</font></p>"
        "<p>Заменяет в начале строки</p><p>"
        "<i>пробелы</i>|<i>звездочка</i>|<i>число</i>|<i>пробелы</i>|<i>круглая скобка</i>|<i>пробелы</i></p>"
        "<p>на <i>звездочку</i> и удаляет</p>"
        "<p><i>пробелы</i>|<i>число</i>|<i>пробелы</i>|<i>круглая скобка</i>|<i>пробелы</i></p></body></html>");
    ui->del_space->setToolTip(
        "<html><head/><body><p>Удаляет <i>пробелы</i></p>"
        "<ol><li>в начале строки</li>"
        "<li>после <i>звездочки</i> в начале строки</li>"
        "<li>после <i>знака вопрса</i> в начале строки</li>"
        "<li>в конце строки</li></ol></body></html>");

    ui->repl_html->setToolTip(
        "<html><head/><body><p>Заменяет:</p>"
        "<p>&amp; &rarr; &amp;amp;</p><p>&lt; &rarr; &amp;lt;</p><p>&gt; &rarr; &amp;gt;</p></body></html>");
    ui->make_tags->setToolTip(
        "<html><head/><body><p>Заменяет</p>"
        "<p>&amp;lt; &rarr; &lt;</p><p>&amp;gt; &rarr; &gt;</p></body></html>");

    ui->replfromto->setToolTip("<html><head/><body><p>Заменяет <i>фразу</i> на <i>фразу</i></p></body></html>");

    ui->repl_split_numsk->setToolTip(
        "<html><head/><body><p>Заменяет</p><p>"
        "<i>пробелы</i>|<i>числа</i>|<i>пробелы</i>|<i>круглая скобка</i></p>"
        "<p>на <i>перевод строки</i></p></body></html>");
    ui->repl_split_alpha->setToolTip(
        "<html><head/><body><p>Заменяет</p><p>"
        "<i>пробелы</i>|<i>числа</i>|<i>пробелы</i>|<i>круглая скобка</i></p>"
        "<p>на <i>перевод строки</i></p></body></html>");
    // построчные замены
    // line replacements
    ui->repl_to_quiz->setToolTip(
        "<html><head/><body><p>Заменяет <i>указанную фразу</i> в начале строки</p>"
        "<p>на <i>знак вопроса</i></p></body></html>");
    ui->repl_answer->setToolTip(
        "<html><head/><body><p>Удаляет <i>указанную фразу</i> в начале строки</p></body></html>");
    ui->repl_to_correct->setToolTip(
        "<html><head/><body><p>Заменяет <i>указанную фразу</i> в конце строки</p>"
        "<p>на <i>звездочку</i></p></body></html>");
}

DialogReplace::~DialogReplace() { delete ui; }
