#include "highlighterdialog.h"
#include "mainwindow.h"
#include "myhighlighter.h"
#include "textdialog.h"
#include "ui_mainwindow.h"
#include <dialogreplace.h>
#include <QDateTime>
#include <QDesktopServices>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QTextStream>
#include "dialogconfig.h"
#include <ui_dialogreplace.h>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //    ui->toolBar->setVisible(false);
    iniFile           = new QSettings("TSU", "TestConvert");
    last_dir          = iniFile->value("dirs/last", QDir::currentPath() ).toString();
    search            = "";
    treePositionLabel = new QLabel();
    ui->statusBar->addPermanentWidget(treePositionLabel);
    tolerance_string = new QLabel(tr("Tolerance: 1/") );
    tolerance        = new QLineEdit("200");
    ui->toolBar->addWidget(tolerance_string);
    ui->toolBar->addWidget(tolerance);
    highlighter = new MyHighlighter(ui->plain->document() );

    // settings
    usecase                = false;
    default_question_price = "10";


    QActionGroup *number_type = new QActionGroup(this);


    number_type->addAction(ui->menuNumber_answersNo);
    number_type->addAction(ui->menuNumber_answers_a);
    number_type->addAction(ui->menuNumber_answers_A);
    number_type->addAction(ui->menuNumber_answers_1);
    number_type->addAction(ui->menuNumber_answers_i);
    number_type->addAction(ui->menuNumber_answers_I);
    // TODO: load colors
#ifdef _DEBUG
    ui->plain->setPlainText(
        "# Section\n"
        "@ Subsection\n"
        "? Question choice\n"
        "or multichoice\n"
        "*correct\n"
        "-incorrect\n"
        "?10%%Question numerical or shortanswer\n"
        "* answer\n"
        "?10%%2+2=\n"
        "*100%%4:0.1\n"
        "*50%%4:0.5\n"
        "? Question matching\n"
        "* option1->value1\n"
        "* option2->value2\n"
        "$ Clozed question intro\n"
        "? Subquestion choice or multichoice\n"
        "* correct\n"
        "incorrect\n"
        "? Subquestion numerical or shortanswer\n"
        "* answer\n"
        "? Subquestion matching\n"
        "* option1->value1\n"
        "* option2->value2\n"
        );
#endif
}
MainWindow::~MainWindow()
{
    iniFile->setValue("dirs/last", last_dir);
    delete treePositionLabel;
    delete iniFile;
    delete ui;
}


void MainWindow::update_format()
{
    if (ui->actionAnswer_marker->isChecked() )
    {
        ui->actionAllow_multiline->setEnabled(true);
    } else {
        ui->actionAllow_multiline->setChecked(false);
        ui->actionAllow_multiline->setEnabled(false);
    }

    highlighter->update_re(ui->actionAllow_multiline->isChecked(), ui->actionAnswer_marker->isChecked() );
}


void MainWindow::resizeEvent(QResizeEvent *)
{
    int w = ui->tree->width();

    ui->tree->setColumnWidth(0, w / 4);
    ui->tree->setColumnWidth(1, w / 2);
}


/// Analyse text from ui->plain and make ui->tree
void MainWindow::on_actionAnalyse_triggered()
{
    ui->tree->clear();


    bool            markers = ui->actionAnswer_marker->isChecked(), multiline = ui->actionAllow_multiline->isChecked();
    QTreeWidgetItem *top = nullptr, *child = nullptr, *quest = nullptr;


    top = new QTreeWidgetItem(QStringList() << tr("section") << tr("Course") );
    ui->tree->addTopLevelItem(top);


    int         i = 0;
    QString     s;
    QStringList data = ui->plain->document()->toPlainText().split("\n");


    while (i < data.size() )
    {
        // get string
        s = data.at(i);
        // Returns a string that has whitespace removed from the start and the end, and that has
        // each sequence of internal whitespace replaced with a single space.
        // And replace \t and &nbsp;
        s = s.replace("&nbsp;", " ").replace("\t", "    ").simplified();

        // test 1
        if ( (s == "") || (s == "[[br]]") )
        {
            // skip
            i++;
            continue;
        }

        if (s.at(0) == '?')
        {
            quest = make_question(data, i, markers, multiline);

            if (quest)
            {
                top->addChild(quest);
            } else {
                return;
            }
        } else if (s.at(0) == '#')
        {
            top = new QTreeWidgetItem(QStringList() << tr("section") << s.mid(1) );
            ui->tree->addTopLevelItem(top);
            i++;
            continue;
        } else if (s.at(0) == '@')
        {
            if (ui->tree->indexOfTopLevelItem(top) == -1)
            {
                top = top->parent();
            }

            child = new QTreeWidgetItem(QStringList() << tr("theme") << s.mid(1) );
            top->addChild(child);
            top = child;
            i++;
            continue;
        } else if (s.at(0) == '$')
        {
            if (ui->tree->indexOfTopLevelItem(top) == -1)
            {
                top = top->parent();
            }

            child = new QTreeWidgetItem(QStringList() << tr("ticket") << s.mid(1) );
            top->addChild(child);
            top = child;
            i++;
            continue;
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Incorrect string #%2: %1").arg(s).arg(i + 1) );
            ui->plain->setTextCursor(ui->plain->document()->find(s) );

            return;
        }
    }
    ui->tree->expandAll();
    on_tree_itemSelectionChanged();
} // MainWindow::on_actionAnalyse_triggered


bool MainWindow::is_answer(QTreeWidgetItem *item) const
{
    return(item->text(0) == tr("option") || item->text(0) == tr("correct")
           || item->text(0) == tr("incorrect") );
}


bool MainWindow::is_question(QTreeWidgetItem *item) const
{
    return(item->text(0) == tr("info") || item->text(0) == tr("essay")
           || item->text(0) == tr("map") || item->text(0) == tr("text")
           || item->text(0) == tr("number") || item->text(0) == tr("choice")
           || item->text(0) == tr("multichoice") );
}


bool MainWindow::is_section(QTreeWidgetItem *item) const
{
    return(item->text(0) == tr("section") || item->text(0) == tr("ticket")
           || item->text(0) == tr("theme") );
}


QTreeWidgetItem *MainWindow::make_question(QStringList &data, int &index, bool markers, bool multiline)
{
    // get string
    QString s = data.at(index).mid(1); // remove ?
    QString price;
    bool    noUnsort = false;

    qWarning(s.toStdString().c_str() );

    if (s.at(0) == '!')
    {
        noUnsort = true;
        s        = s.mid(1);
    }

    if ( (s.indexOf("%%") > -1) && (s.indexOf("%%") < 5) )
    {
        price = s.left(s.indexOf("%%") );
        s     = s.mid(s.indexOf("%%") + 2);
    } else {
        price = default_question_price;
    }

    // Returns a string that has whitespace removed from the start and the end, and that has
    // each sequence of internal whitespace replaced with a single space.
    // And replace \t and &nbsp;
    s = s.replace("&nbsp;", " ").replace("\t", "    ").simplified();


    QTreeWidgetItem *quest = new QTreeWidgetItem(QStringList() << tr("unc") << s << "" << price);


    if (noUnsort)
    {
        quest->setText(2, "!");
    }

    index++;

    if (multiline)
    {
        while (index < data.size() )
        {
            s = data.at(index);
            s = s.replace("&nbsp;", " ").trimmed();

            if (s != "" && (s[0] == '*' || s[0] == '-' || s[0] == '+' || s[0] == '?'|| s[0] == '@'|| s[0] == '#') )
            {
                break;
            }

            quest->setText(1, quest->text(1) + "\n" + s);
            index++;
        }
    }


    //
    QStringList answers;
    int         correctcount = 0;


    while (index < data.size() )
    {
        s = data.at(index);
        s = s.replace("&nbsp;", " ").trimmed();

        // ответы кончились?
        if ( (s.at(0) == '#') || (s.at(0) == '@') || (s.at(0) == '?') || (s.at(0) == '$') )
        {
            break;
        }

        // NOTE: Возможны варианты
        // ----------|---|---|---|
        // multiline | f | f | t |
        // markers   | f | t | t |
        // ----------|---|---|---|
        if (markers && answers.empty() )
        {
            // это первый ответ
            // проверить наличие маркера
            if (s.isEmpty() )
            {
                index++;
                continue;
            }

            if (s[0] != '*' && s[0] != '+' && s[0] != '-'&& s[0] != '?')
            {
                QMessageBox::warning(this, tr("Error"), tr("Incorrect string #%2: %1").arg(s).arg(index + 1) );
                ui->plain->setTextCursor(ui->plain->document()->find(s) );

                return(nullptr);
            }
        }

        // в ответах не может быть пустой строки
        if ( (s == "") || (s == "[[br]]") )
        {
            index++;
            continue;
        }


        // проверка на правильность
        if (markers)
        {
            if (s.at(0) == '*')
            {
                correctcount++;
                answers.append(s);
            } else if (s.at(0) == '+')
            {
                correctcount++;
                s = "*"+s.mid(1).trimmed();
                answers.append(s);
            } else if (s.at(0) == '-')
            {
                s = s.mid(1);

                if (s.at(0) == '*')
                {
                    correctcount++;
                }

                answers.append(s.trimmed());
            } else {
                if (multiline)
                {
                    answers.back() += "\n" + s;
                } else {
                    QMessageBox::warning(this, tr("Error"), tr("Incorrect string #%2: %1").arg(s).arg(index + 1) );
                    ui->plain->setTextCursor(ui->plain->document()->find(s) );

                    return(nullptr);
                }
            }
        } else {
            if (s.at(0) == '*')
            {
                correctcount++;
            }

            answers.append(s);
        }

        index++;
    }


    bool infoessay = ui->format_essay->isChecked();


    // test answers
    if (answers.size() == 0)
    {
        // no answers
        if (infoessay)
        {
            quest->setText(0, tr("essay") );
        } else if (ui->format_info->isChecked() )
        {
            quest->setText(0, tr("info") );
        }

        if (ui->actionRemove_incorrect->isChecked() )
        {
            delete quest;

            return(nullptr);
        } else {
            return(quest);
        }
    }

    if (is_map(answers) && ui->format_matching->isChecked() )
    {
        // suspicion of map
        quest->setText(0, tr("map") );

        for (int k = 0; k < answers.size(); k++)
        {
            s = answers.at(k);

            if (s.at(0) == '*')
            {
                s = s.mid(1);
            }

            quest->addChild(new QTreeWidgetItem(
                                QStringList() << tr("option") <<
                                    s.mid(0, s.indexOf("->") ) << s.mid(s.indexOf("->") + 2) ) );
        }
    } else {
        // test normal
        bool    number = false;
        QString p, t;
        QString s2;
        QString format;


        if ( (correctcount == answers.size() )
             && (!ui->action_enableAll->isChecked() || correctcount == 1) )
        {
            if (ui->format_shortanswer->isChecked() )
            {
                format = tr("text");
            }

            number = true;

            for (int k = 0; k < answers.size(); k++)
            {
                s = answers.at(k);

                if (!parse_answer(s, p, s2, t, true) )
                {
                    number = false;
                }
            }

            if (number && ui->format_numerical->isChecked() )
            {
                format = tr("number");
            }
        }

        if (!format.isEmpty() )
        {
        } else if (correctcount == 1 && ui->format_choice->isChecked() )
        {
            format = tr("choice");
        } else if (ui->format_multichoice->isChecked() )
        {
            format = tr("multichoice");
        }

        if (format.isEmpty() )
        {
            if (ui->actionRemove_incorrect->isChecked() )
            {
                delete quest;

                return(nullptr);
            }
        } else {
            quest->setText(0, format);
        }

        number = (format == tr("number") );

        for (int k = 0; k < answers.size(); k++)
        {
            s = answers.at(k);
            parse_answer(s, p, s2, t, number);

            if (s.at(0) == '*')
            {
                if (number)
                {
                    quest->addChild(new QTreeWidgetItem(QStringList() << tr("correct") << s2 << t << p) );
                } else {
                    quest->addChild(new QTreeWidgetItem(QStringList() << tr("correct") << s2 << "" << p) );
                }
            } else {
                quest->addChild(new QTreeWidgetItem(QStringList() << tr("incorrect") << s2 << "" << p) );
            }
        }
    }

    return(quest);
} // MainWindow::make_question


/// line-of-answer analysis
/// \param s string
/// \param price question price
/// \param text answer text
/// \param tolerance answer accuracy
/// \return is number
bool MainWindow::parse_answer(QString s, QString &price, QString &text, QString &tolerance, bool number)
{
    bool ok, ok2;

    if (s.leftRef(1) == "*")
    {
        s = s.mid(1);
    }

    if ( (s.indexOf("%%") != -1) && (s.indexOf("%%") < 15) )
    {
        price = s.left(s.indexOf("%%") );
        price.toDouble(&ok);

        if (ok)
        {
            s = s.mid(s.indexOf("%%") + 2);
        } else {
            price = "";
        }
    } else {
        price = "";
    }

    if (number && s.count(":") == 1)
    {
        QString s1 = s.left(s.indexOf(":") );
        QString s2 = s.mid(s.indexOf(":") + 1);


        s1.toDouble(&ok);
        s2.toDouble(&ok2);

        if (ok && ok2)
        {
            text      = s1;
            tolerance = s2;

            return(true);
        } else {
            text      = s;
            tolerance = "";

            return(false);
        }
    } else {
        s.toDouble(&ok);
        tolerance = "";
        text      = s;

        return(ok);
    }
} // MainWindow::parse_answer


bool MainWindow::is_map(const QStringList &answers) const
{
    for (auto s : answers)
    {
        if (s.indexOf("->") == -1)
        {
            return(false);
        }
    }

    return(true);
}


/// Open File
/// \note is supposed to use html, but should also work with plain text
void MainWindow::on_actionOpen_triggered()
{
    QString fname = QFileDialog::getOpenFileName(this, tr("Open"), last_dir, tr("html (*.htm;*.html);ANY FILE (*)") );

    if (!fname.isEmpty() )
    {
        images.clear();


        QFile     f(fname);
        QFileInfo fi(fname);


        last_dir = fi.dir().path();
        f.open(QIODevice::ReadOnly);


        QTextStream in(&f);


        in.setCodec("windows-1251");


        QString s = in.readAll();


        if (s.indexOf("text/html; charset=") > 0)
        {
            s = s.mid(s.indexOf("text/html; charset="), 50);
            s = s.mid(s.indexOf("=") + 1);
            s = s.left(s.indexOf("\"") );
            in.setCodec(s.toStdString().c_str() );
            in.seek(0);
            s = in.readAll();
        }

        ui->plain->setHtml(s);
    }
}


/// Save lino to xml-file
/// \param stream xml-file object
/// \param txt string
/// \param basepath path to image files
void MainWindow::writeText(QXmlStreamWriter &stream, QString txt, QString basepath)
{
    QStringList images;
    // список файлов
    int         num;

    while (txt.indexOf("!(") > -1)
    {
        QString s1 = txt.mid(0, txt.indexOf("!(") );
        QString s2 = txt.mid(txt.indexOf("!(") + 2);
        QString s3 = s2.mid(s2.indexOf(")") + 1);


        s2 = s2.mid(0, s2.indexOf(")") );

        if (!images.contains(s2) )
        {
            num = images.size();
            images.append(s2);
        } else {
            num = images.indexOf(s2);
        }

        s2  = s2.mid(s2.lastIndexOf('.') );
        txt = s1 + "<img src=\"@@PLUGINFILE@@/" + QString("%1%2").arg(num).arg(s2) + "\">" + s3;
    }
    txt = txt.replace("[[br]]", "<br />")
              .replace("[[sub]]", "<sub>")
              .replace("[[/sub]]", "</sub>")
              .replace("[[sup]]", "<sup>")
              .replace("[[/sup]]", "</sup>");
    stream.writeTextElement("text", txt);
    num = 0;

    for (auto fn : images)
    {
        QFile file(fn);


        if (file.exists() )
        {
            file.open(QIODevice::ReadOnly);
        } else {
            file.setFileName(basepath + "/" + fn);

            if (file.exists() )
            {
                file.open(QIODevice::ReadOnly);
            } else {
                QMessageBox::warning(this, tr("Error"), tr("File %1 not found").arg(fn) );
            }
        }

        stream.writeStartElement("file");
        stream.writeAttribute("name", QString("%1%2").arg(num).arg(fn.mid(fn.lastIndexOf('.') ) ) );
        stream.writeAttribute("path", "/");
        stream.writeAttribute("encoding", "base64");
        stream.writeCharacters(file.readAll().toBase64() );
        stream.writeEndElement();
        num++;
    }
} // MainWindow::writeText


/// Remove tags and images from document line
/// \param s original string
/// \return modified string
QString to_title(QString s)
{
    QRegExp notags("\\<.*\\>");

    notags.setMinimal(true);
    s = s.replace(notags, "");


    QRegExp notags2("&lt;.*&gt;");


    notags2.setMinimal(true);
    s = s.replace(notags2, "");


    QRegExp nospec("&.*;");


    nospec.setMinimal(true);
    s = s.replace(nospec, "");

    if (s.length() > 100)
    {
        s = s.left(90) + "...";
    }

    return(s);
}


void MainWindow::on_actionSet_work_dir_triggered()
{
    QString fn = QFileDialog::getExistingDirectory(this, tr("Select image dir"), QString() );

    if (!fn.isEmpty() )
    {
        last_dir = fn;
    }
}


/// Export questions
void MainWindow::on_actionExport_triggered()
{
    double ktolerance;
    bool   btolerance;

    btolerance = ui->actionFixedAccuracy->isChecked();


    bool ok;


    ktolerance = tolerance->text().toDouble(&ok);

    if (!ok)
    {
        QMessageBox::critical(this, tr("Error"), tr("Incorrect tolerance value") );

        return;
    }


    QString fname = QFileDialog::getSaveFileName(this, tr("Save"), last_dir + "/quiz.xml", tr("xml-file (*.xml)") );


    if (!fname.isEmpty() )
    {
        /*
         * Настройка
         */
        QString   open_xml;
        QString   close_xml;
        QString   text;
        QString   number;
        QString   multichoice;
        QString   choice;
        QString   theme;
        QString   section;
        QString   map;
        QString   info;
        QString   answer;
        QString   numanswer;
        QString   mapanswer;
        // -------------------------------
        QFileInfo fi(fname);


        last_dir = fi.dir().path();


        //        bool                     as_multi = ui->actionChoiceAsMultichoice->isChecked();    // false; //экспорт choice как multichoice
        //        bool                     as_text  = ui->actionNumericalAsShortanswer->isChecked(); // false; //экспорт number как text
        QList<QTreeWidgetItem *> themes;
        QTreeWidgetItem          *top;
        QFile                    f;


        f.setFileName(fname);
        f.open(QIODevice::WriteOnly);


        QXmlStreamWriter stream(&f);


        stream.setAutoFormatting(true);
        stream.writeStartDocument();
        stream.writeStartElement("quiz");

        for (int i = 0; i < ui->tree->topLevelItemCount(); i++)
        {
            top = ui->tree->topLevelItem(i);
            themes << top;

            if (!process_tree(stream, top, ktolerance, btolerance) )
            {
                export_error();
                break;
            }
        }

        stream.writeEndElement(); // bookmark
        stream.writeEndDocument();
        f.close();
    }
} // MainWindow::on_actionExport_triggered


void MainWindow::export_error()
{
}


bool MainWindow::process_question(QXmlStreamWriter &stream, QTreeWidgetItem *item, double ktolerance, bool btolerance)
{
    if (item->text(0) == tr("info") )
    {
        return(write_info(stream, item) );
    } else if (item->text(0) == tr("essay") )
    {
        return(write_essay(stream, item) );
    } else if (item->text(0) == tr("map") )
    {
        return(write_matching(stream, item) );
    }

    if (item->text(0) == tr("text") )
    {
        return(write_shortanswer(stream, item) );
    } else if (item->text(0) == tr("number") )
    {
        return(write_numerical(stream, item, ktolerance, btolerance) );
    } else if (item->text(0) == tr("multichoice") )
    {
        return(write_multichoice(stream, item) );
    } else if (item->text(0) == tr("choice") )
    {
        return(write_choice(stream, item) );
    } else if (item->text(0) == tr("ticket") )
    {
        return(write_close(stream, item, ktolerance, btolerance) );
    } else {
        show_error(item, tr("unknown type") );

        return(false);
    }
}


bool MainWindow::write_close(QXmlStreamWriter &stream, QTreeWidgetItem *item, double ktolerance, bool btolerance)
{
    QStringList questions;

    questions << item->text(1);


    bool ok;


    stream.writeStartElement("question");
    stream.writeAttribute("type", "cloze");
    stream.writeStartElement("name");
    stream.writeTextElement("text", to_title(questions[0]) );
    stream.writeEndElement();
    stream.writeStartElement("questiontext");
    stream.writeAttribute("format", "moodle_auto_format");

    //                    stream.writeTextElement("text", item->text(1));
    for (int i = 0; i < item->childCount(); i++)
    {
        questions << process_subquestion(item->child(i), ktolerance, btolerance, ok);

        if (!ok)
        {
            return(false);
        }
    }

    writeText(stream, questions.join("<br />"), last_dir);
    // add image
    stream.writeEndElement();
    stream.writeStartElement("generalfeedback");
    stream.writeTextElement("text", "");
    stream.writeEndElement();
    stream.writeTextElement("defaultgrade", "0");
    stream.writeTextElement("penalty", "0");
    stream.writeTextElement("hidden", "0");
    stream.writeTextElement("shuffleanswers", "1");
    stream.writeEndElement();

    return(true);
} // MainWindow::write_close


bool MainWindow::write_info(QXmlStreamWriter &stream, QTreeWidgetItem *item)
{
    stream.writeStartElement("question");
    stream.writeAttribute("type", "description");
    stream.writeStartElement("name");
    stream.writeTextElement("text", to_title(item->text(1) ) );
    stream.writeEndElement();
    stream.writeStartElement("questiontext");
    stream.writeAttribute("format", "moodle_auto_format");
    //                    stream.writeTextElement("text", item->text(1));
    writeText(stream, item->text(1), last_dir);
    // add image
    stream.writeEndElement();
    stream.writeStartElement("generalfeedback");
    stream.writeTextElement("text", "");
    stream.writeEndElement();
    stream.writeTextElement("defaultgrade", "0");
    stream.writeTextElement("penalty", "0");
    stream.writeTextElement("hidden", "0");
    stream.writeTextElement("shuffleanswers", "0");
    stream.writeEndElement();

    return(true);
}


bool MainWindow::write_essay(QXmlStreamWriter &stream, QTreeWidgetItem *item)
{
    stream.writeStartElement("question");
    stream.writeAttribute("type", "essay");
    stream.writeStartElement("name");
    stream.writeTextElement("text", to_title(item->text(1) ) );
    stream.writeEndElement();
    stream.writeStartElement("questiontext");
    stream.writeAttribute("format", "moodle_auto_format");
    //                    stream.writeTextElement("text", item->text(1));
    writeText(stream, item->text(1), last_dir);
    // add image
    stream.writeEndElement();
    stream.writeStartElement("generalfeedback");
    stream.writeTextElement("text", "");
    stream.writeEndElement();
    stream.writeTextElement("defaultgrade", item->text(3) );
    stream.writeTextElement("penalty", "1");
    stream.writeTextElement("hidden", "0");
    stream.writeTextElement("responseformat", "editor");
    stream.writeTextElement("responserequired", "0");
    stream.writeTextElement("responsefieldlines", "40");
    stream.writeTextElement("attachments", "-1");
    stream.writeTextElement("attachmentsrequired", "0");
    stream.writeTextElement("maxbytes", "0");
    stream.writeEndElement();

    return(true);
}


QString MainWindow::format_info(QTreeWidgetItem *item, bool &ok)
{
    ok = true;

    return(item->text(1) );
}


bool MainWindow::write_choice(QXmlStreamWriter &stream, QTreeWidgetItem *item)
{
    stream.writeStartElement("question");
    stream.writeAttribute("type", "multichoice");
    stream.writeStartElement("name");
    stream.writeTextElement("text", to_title(item->text(1) ) );
    stream.writeEndElement();
    stream.writeStartElement("questiontext");
    stream.writeAttribute("format", "moodle_auto_format");
    writeText(stream, item->text(1), last_dir);
    // add image
    stream.writeEndElement();
    stream.writeStartElement("generalfeedback");
    stream.writeTextElement("text", "");
    stream.writeEndElement();
    stream.writeTextElement("single", "true");
    stream.writeTextElement("defaultgrade", item->text(3) );
    stream.writeTextElement("penalty", "1");
    stream.writeTextElement("hidden", "0");

    if (ui->actionShuffle_answers->isChecked() && item->text(2) != "!")
    {
        stream.writeTextElement("shuffleanswers", "1");
    } else {
        stream.writeTextElement("shuffleanswers", "0");
    }

    if (ui->menuNumber_answersNo->isChecked() )
    {
        stream.writeTextElement("answernumbering", "none");
    } else if (ui->menuNumber_answers_1->isChecked() )
    {
        stream.writeTextElement("answernumbering", "123");
    } else if (ui->menuNumber_answers_A->isChecked() )
    {
        stream.writeTextElement("answernumbering", "ABCD");
    } else if (ui->menuNumber_answers_a->isChecked() )
    {
        stream.writeTextElement("answernumbering", "abc");
    } else if (ui->menuNumber_answers_I->isChecked() )
    {
        stream.writeTextElement("answernumbering", "IIII");
    } else if (ui->menuNumber_answers_i->isChecked() )
    {
        stream.writeTextElement("answernumbering", "iii");
    }


    //<answernumbering>abc</answernumbering>
    //<answernumbering>none</answernumbering>
    /*<select class="custom-select
     *                     " name="answernumbering" id="id_answernumbering" data-initial-value="none">
     *          <option value="abc">a., b., c., ...</option>
     *          <option value="ABCD">A., B., C., ...</option>
     *          <option value="123">1., 2., 3., ...</option>
     *          <option value="iii">i., ii., iii., ...</option>
     *          <option value="IIII">I., II., III., ...</option>
     *          <option value="none" selected="">Не нумеровать</option>
     *      </select>*/
    // answ
    int correct = 0;


    for (int k = 0; k < item->childCount(); k++)
    {
        stream.writeStartElement("answer");

        if (item->child(k)->text(0) == tr("correct") )
        {
            if (item->child(k)->text(3) != "")
            {
                stream.writeAttribute("fraction", item->child(k)->text(3) );
            } else {
                stream.writeAttribute("fraction", "100");
            }

            correct++;
        } else {
            if (item->child(k)->text(3) != "")
            {
                stream.writeAttribute("fraction", item->child(k)->text(3) );
            } else {
                stream.writeAttribute("fraction", "0");
            }
        }

        writeText(stream, item->child(k)->text(1), last_dir);
        stream.writeStartElement("feedback");
        stream.writeTextElement("text", "");
        stream.writeEndElement();
        stream.writeEndElement();
    }

    if (correct == 0)
    {
        show_error(item, tr("No correct answer") );

        return(false);
    }

    stream.writeEndElement();

    return(true);
} // MainWindow::write_choice


QString MainWindow::format_choice(QTreeWidgetItem *item, bool &ok)
{
    QString     ret = item->text(1);
    QStringList answers;

    ret += "<br/>{" + item->text(3) + ":MULTICHOICE_V:";


    int     correct = 0;
    QString price;


    for (int k = 0; k < item->childCount(); k++)
    {
        if (item->child(k)->text(0) == tr("correct") )
        {
            if (item->child(k)->text(3) != "")
            {
                price = item->child(k)->text(3);
            } else {
                price = "100";
            }

            answers << "%" + price + "%" + item->child(k)->text(1).replace("}", "\\}");
            correct++;
        } else {
            if (item->child(k)->text(3) != "")
            {
                price = item->child(k)->text(3);
            } else {
                price = "0";
            }

            answers << "%" + price + "%" + item->child(k)->text(1).replace("}", "\\}");
        }
    }

    ret += answers.join("~") + "}";

    if (correct == 0)
    {
        show_error(item, tr("No right answers.") );
        ok = false;
    } else {
        ok = true;
    }

    return(ret);
} // MainWindow::format_choice


bool MainWindow::write_multichoice(QXmlStreamWriter &stream, QTreeWidgetItem *item)
{
    stream.writeStartElement("question");
    stream.writeAttribute("type", "multichoice");
    stream.writeStartElement("name");
    stream.writeTextElement("text", to_title(item->text(1) ) );
    stream.writeEndElement();
    stream.writeStartElement("questiontext");
    stream.writeAttribute("format", "moodle_auto_format");
    writeText(stream, item->text(1), last_dir);
    // add image
    stream.writeEndElement();
    stream.writeStartElement("generalfeedback");
    stream.writeTextElement("text", "");
    stream.writeEndElement();
    stream.writeTextElement("single", "false");
    stream.writeTextElement("defaultgrade", item->text(3) );
    stream.writeTextElement("penalty", "1");
    stream.writeTextElement("hidden", "0");

    if (ui->actionShuffle_answers->isChecked() && item->text(2) != "!")
    {
        stream.writeTextElement("shuffleanswers", "1");
    } else {
        stream.writeTextElement("shuffleanswers", "0");
    }

    if (ui->menuNumber_answersNo->isChecked() )
    {
        stream.writeTextElement("answernumbering", "none");
    } else if (ui->menuNumber_answers_1->isChecked() )
    {
        stream.writeTextElement("answernumbering", "123");
    } else if (ui->menuNumber_answers_A->isChecked() )
    {
        stream.writeTextElement("answernumbering", "ABCD");
    } else if (ui->menuNumber_answers_a->isChecked() )
    {
        stream.writeTextElement("answernumbering", "abc");
    } else if (ui->menuNumber_answers_I->isChecked() )
    {
        stream.writeTextElement("answernumbering", "IIII");
    } else if (ui->menuNumber_answers_i->isChecked() )
    {
        stream.writeTextElement("answernumbering", "iii");
    }


    // answ
    int correct = 0, incorrect = 0;


    for (int k = 0; k < item->childCount(); k++)
    {
        if (item->child(k)->text(0) == tr("correct") )
        {
            correct++;
        } else {
            incorrect++;
        }
    }

    if ( (correct == 0) )
    {
        show_error(item, tr("No right answers.") );

        return(false);
    }

    if ( (incorrect == 0) && !ui->action_enableAll->isChecked() )
    {
        show_error(item, tr("No wrong answers.") );

        return(false);
    }

    // TODO: answer price
    for (int k = 0; k < item->childCount(); k++)
    {
        stream.writeStartElement("answer");

        if (item->child(k)->text(0) == tr("correct") )
        {
            stream.writeAttribute("fraction", QString("%1").arg(100.0 / correct) );
        } else {
            if (ui->action_bigPenalty->isChecked() )
            {
                stream.writeAttribute("fraction", QString("%1").arg(-100) );
            } else {
                stream.writeAttribute("fraction", QString("%1").arg(-100.0 / incorrect) );
            }
        }

        writeText(stream, item->child(k)->text(1), last_dir);
        stream.writeStartElement("feedback");
        stream.writeTextElement("text", "");
        stream.writeEndElement();
        stream.writeEndElement();
    }

    stream.writeEndElement();

    return(true);
} // MainWindow::write_multichoice


QString MainWindow::format_multichoice(QTreeWidgetItem *item, bool &ok)
{
    QString     ret = item->text(1);
    QStringList answers;

    ret += "<br/>{" + item->text(3) + ":MULTIRESPONSE:";
    ok   = true;


    int correct = 0, incorrect = 0;


    for (int k = 0; k < item->childCount(); k++)
    {
        if (item->child(k)->text(0) == tr("correct") )
        {
            correct++;
        } else {
            incorrect++;
        }
    }

    if ( (correct == 0) )
    {
        show_error(item, tr("No right answers.") );
        ok = false;

        return("");
    }

    if ( (incorrect == 0) && !ui->action_enableAll->isChecked() )
    {
        show_error(item, tr("No wrong answers.") );
        ok = false;

        return("");
    }

    // TODO: answer price
    for (int k = 0; k < item->childCount(); k++)
    {
        if (item->child(k)->text(0) == tr("correct") )
        {
            answers << "%" + QString("%1").arg(100.0 / correct) + "%" + item->child(k)->text(1).replace("}", "\\}");
        } else {
            answers << "%" + QString("%1").arg(-100.0 / incorrect) + "%" + item->child(k)->text(1).replace("}", "\\}");
        }
    }

    ret += answers.join("~") + "}";

    return(ret);
} // MainWindow::format_multichoice


bool MainWindow::write_numerical(QXmlStreamWriter &stream, QTreeWidgetItem *item, double ktolerance, bool btolerance)
{
    stream.writeStartElement("question");
    stream.writeAttribute("type", "numerical");
    stream.writeStartElement("name");
    stream.writeTextElement("text", to_title(item->text(1) ) );
    stream.writeEndElement();
    stream.writeStartElement("questiontext");
    stream.writeAttribute("format", "moodle_auto_format");
    writeText(stream, item->text(1), last_dir);
    // add image
    stream.writeEndElement();
    stream.writeStartElement("generalfeedback");
    stream.writeTextElement("text", "");
    stream.writeEndElement();
    stream.writeTextElement("defaultgrade", item->text(3) );
    stream.writeTextElement("penalty", "1");
    stream.writeTextElement("hidden", "0");
    stream.writeTextElement("shuffleanswers", "1");


    // answ
    double qtolerance;


    for (int k = 0; k < item->childCount(); k++)
    {
        if (item->child(k)->text(0) == tr("correct") )
        {
            stream.writeStartElement("answer");

            if (item->child(k)->text(3) != "")
            {
                stream.writeAttribute("fraction", item->child(k)->text(3) );
            } else {
                stream.writeAttribute("fraction", "100");
            }

            if (item->child(k)->text(2) != "")
            {
                qtolerance = item->child(k)->text(2).toDouble();
            } else if (btolerance)
            {
                qtolerance = ktolerance;
            } else {
                qtolerance = abs(item->child(k)->text(1).toDouble() / ktolerance);
            }

            stream.writeAttribute("tolerance", QString("%1").arg(qtolerance) );
            stream.writeTextElement("text", item->child(k)->text(1) );
            stream.writeStartElement("feedback");
            stream.writeTextElement("text", "");
            stream.writeEndElement();
            stream.writeEndElement();
        }
    }

    stream.writeEndElement();

    return(true);
} // MainWindow::write_numerical


QString MainWindow::format_numerical(QTreeWidgetItem *item, double ktolerance, bool btolerance, bool &ok)
{
    QString     ret = item->text(1);
    QStringList answers;

    ret += "<br/>{" + item->text(3) + ":NUMERICAL:";
    ok   = true;


    //
    double  qtolerance;
    QString price;


    for (int k = 0; k < item->childCount(); k++)
    {
        if (item->child(k)->text(0) == tr("correct") )
        {
            if (item->child(k)->text(2) != "")
            {
                qtolerance = item->child(k)->text(2).toDouble();
            } else if (btolerance)
            {
                qtolerance = ktolerance;
            } else {
                qtolerance = abs(item->child(k)->text(1).toDouble() / ktolerance);
            }

            if (item->child(k)->text(3) != "")
            {
                price = item->child(k)->text(3);
            } else {
                price = "100";
            }

            answers << "%" + price + "%" + item->child(k)->text(1) + ":" + QString("%1").arg(qtolerance);
        }
    }

    //
    ret += answers.join("~") + "}";

    return(ret);
} // MainWindow::format_numerical


bool MainWindow::write_shortanswer(QXmlStreamWriter &stream, QTreeWidgetItem *item)
{
    stream.writeStartElement("question");
    stream.writeAttribute("type", "shortanswer");
    stream.writeStartElement("name");
    stream.writeTextElement("text", to_title(item->text(1) ) );
    stream.writeEndElement();
    stream.writeStartElement("questiontext");
    stream.writeAttribute("format", "moodle_auto_format");
    writeText(stream, item->text(1), last_dir);
    // add image
    stream.writeEndElement();
    stream.writeStartElement("generalfeedback");
    stream.writeTextElement("text", "");
    stream.writeEndElement();
    stream.writeTextElement("defaultgrade", item->text(3) );
    stream.writeTextElement("penalty", "1");
    stream.writeTextElement("hidden", "0");

    if (usecase)
    {
        stream.writeTextElement("usecase", "1");
    } else {
        stream.writeTextElement("usecase", "0");
    }

    stream.writeTextElement("shuffleanswers", "1");

    // answ
    for (int k = 0; k < item->childCount(); k++)
    {
        if (item->child(k)->text(0) == tr("correct") )
        {
            stream.writeStartElement("answer");

            if (item->child(k)->text(3) != "")
            {
                stream.writeAttribute("fraction", item->child(k)->text(3) );
            } else {
                stream.writeAttribute("fraction", "100");
            }

            stream.writeTextElement("text", item->child(k)->text(1) );
            stream.writeStartElement("feedback");
            stream.writeTextElement("text", "");
            stream.writeEndElement();
            stream.writeEndElement();
        }
    }

    stream.writeEndElement();

    return(true);
} // MainWindow::write_shortanswer


QString MainWindow::format_shortanswer(QTreeWidgetItem *item, bool &ok)
{
    QString     ret = item->text(1);
    QStringList answers;
    QString     price;

    if (usecase)
    {
        ret += "<br/>{" + item->text(3) + ":SHORTANSWER_C:";
    } else {
        ret += "<br/>{" + item->text(3) + ":SHORTANSWER:";
    }

    ok = true;

    // answ
    for (int k = 0; k < item->childCount(); k++)
    {
        if (item->child(k)->text(0) == tr("correct") )
        {
            if (item->child(k)->text(3) != "")
            {
                price = item->child(k)->text(3);
            } else {
                price = 100;
            }

            answers << "%" + price + "%" + item->child(k)->text(1);
        }
    }

    //
    ret += answers.join("~") + "}";

    return(ret);
}


bool MainWindow::write_matching(QXmlStreamWriter &stream, QTreeWidgetItem *item)
{
    stream.writeStartElement("question");
    stream.writeAttribute("type", "matching");
    stream.writeStartElement("name");
    stream.writeTextElement("text", to_title(item->text(1) ) );
    stream.writeEndElement();
    stream.writeStartElement("questiontext");
    stream.writeAttribute("format", "moodle_auto_format");
    writeText(stream, item->text(1), last_dir);
    // add image
    stream.writeEndElement();
    stream.writeStartElement("generalfeedback");
    stream.writeTextElement("text", "");
    stream.writeEndElement();
    stream.writeTextElement("defaultgrade", item->text(3) );
    stream.writeTextElement("penalty", "1");
    stream.writeTextElement("hidden", "0");
    stream.writeTextElement("shuffleanswers", "1");

    // answ
    for (int k = 0; k < item->childCount(); k++)
    {
        if (item->child(k)->text(0) == tr("option") )
        {
            stream.writeStartElement("subquestion");
            writeText(stream, item->child(k)->text(1), last_dir);
            stream.writeStartElement("answer");
            stream.writeTextElement("text", item->child(k)->text(2) );
            stream.writeEndElement();
            stream.writeEndElement();
        }
    }

    stream.writeEndElement();

    return(true);
}


QString MainWindow::format_matching(QTreeWidgetItem *item, bool &ok)
{
    QString     ret = item->text(1);
    QStringList answers;

    ret += "<br/>";
    ok   = true;


    // answ
    QStringList left;
    QStringList right;


    for (int k = 0; k < item->childCount(); k++)
    {
        if (item->child(k)->text(0) == tr("option") )
        {
            left << item->child(k)->text(1);
            right << item->child(k)->text(2);
        }
    }

    for (int i = 0; i < left.count(); i++)
    {
        ret += left[i];
        answers.clear();

        for (int j = 0; j < right.count(); j++)
        {
            if (i == j)
            {
                answers << "%100%" + right[j].replace("}", "\\}");
            } else {
                answers << "%0%" + right[j].replace("}", "\\}");
            }
        }

        ret += "{" + item->text(3) + ":MULTICHOICE:" + answers.join("~") + "}";
    }

    //
    return(ret);
} // MainWindow::format_matching


QString MainWindow::process_subquestion(QTreeWidgetItem *item, double ktolerance, bool btolerance, bool &ok)
{
    if (item->text(0) == tr("info") )
    {
        return(format_info(item, ok) );
    } else if (item->text(0) == tr("essay") )
    {
        return("");
    } else if (item->text(0) == tr("map") )
    {
        return(format_matching(item, ok) );
    }

    if (item->text(0) == tr("text") )
    {
        return(format_shortanswer(item, ok) );
    } else if (item->text(0) == tr("number") )
    {
        return(format_numerical(item, ktolerance, btolerance, ok) );
    } else if (item->text(0) == tr("multichoice") )
    {
        return(format_multichoice(item, ok) );
    } else if (item->text(0) == tr("choice") )
    {
        return(format_choice(item, ok) );
    } else {
        show_error(item, tr("unknown type") );
        ok = false;

        return("");
    }
}


bool MainWindow::process_tree(QXmlStreamWriter &stream, QTreeWidgetItem *item, double ktolerance, bool btolerance)
{
    //        stream.writeAttribute("href", "http://qt-project.org/");
    //        stream.writeTextElement("title", "Qt Project");
    //        stream.writeEndElement();    // bookmark
    if (item->text(0) == tr("section") )
    {
        write_section(stream, item);

        for (int i = 0; i < item->childCount(); i++)
        {
            if (!process_tree(stream, item->child(i), ktolerance, btolerance) )
            {
                return(false);
            }
        }
    } else if (item->text(0) == tr("theme") )
    {
        write_theme(stream, item);

        for (int i = 0; i < item->childCount(); i++)
        {
            if (!process_tree(stream, item->child(i), ktolerance, btolerance) )
            {
                return(false);
            }
        }
    } else {
        if (!process_question(stream, item, ktolerance, btolerance) )
        {
            return(false);
        }
    }

    return(true);
}


void MainWindow::write_theme(QXmlStreamWriter &stream, QTreeWidgetItem *item) const
{
    stream.writeStartElement("question");
    stream.writeAttribute("type", "category");
    stream.writeStartElement("category");
    stream.writeTextElement("text", item->parent()->text(1) + "/" + item->text(1) );
    stream.writeEndElement();
    stream.writeEndElement();
}


void MainWindow::write_section(QXmlStreamWriter &stream, QTreeWidgetItem *item) const
{
    stream.writeStartElement("question");
    stream.writeAttribute("type", "category");
    stream.writeStartElement("category");
    stream.writeTextElement("text", item->text(1) );
    stream.writeEndElement();
    stream.writeEndElement();
}


/// Replace in ui->plain
void MainWindow::on_actionReplace_triggered()
{
    DialogReplace dlg;

    // TODO: edit
    if (dlg.exec() )
    {
        // папка для картинок
        //        QString path = QString::number(QDateTime::currentSecsSinceEpoch(), 16);
        //        QDir work(last_dir);    //= QDir::current();
        //        //        last_dir = work;
        //        work.mkpath(path);
        //        path = work.relativeFilePath(path);
        // списки замен
        QString     s = ui->plain->document()->toHtml();
        QStringList lines;
        int         pos = s.indexOf("<body", 0, Qt::CaseInsensitive);
        int         pos2, pos3;


        s = s.mid(pos);
        s = s.mid(s.indexOf(">") + 1);
        s = s.left(s.indexOf("</body", 0, Qt::CaseInsensitive) );

        // TODO: H tag
        // преобразование тегов
        while (true)
        {
            if (s.isEmpty() )
            {
                break;
            }

            pos  = s.indexOf("</p>", 0, Qt::CaseInsensitive);
            pos2 = s.indexOf("</h", 0, Qt::CaseInsensitive);
            pos3 = s.indexOf("</li", 0, Qt::CaseInsensitive);

            if (pos2 != -1 && (pos > pos2 || pos == -1) )
            {
                pos = pos2;
            }

            if (pos3 != -1 && (pos > pos3 || pos == -1) )
            {
                pos = pos3;
            }


            QString p = s.left(pos);


            s = s.mid(pos);
            s = s.mid(s.indexOf(">") + 1);
            p = p.mid(p.indexOf(">") + 1);
            // replace tags
            p = p.replace("<br />", "[[br]]")
                    .replace("<sub>", "[[sub]]")
                    .replace("</sub>", "[[/sub]]")
                    .replace("<sup>", "[[sup]]")
                    .replace("</sup>", "[[/sup]]")
                    .replace("\n", " ");


            QRegularExpression              expression2("<img\\s*[^<]*src\\s*=\\s*\"(.+?)\"[^>]*>");
            QRegularExpressionMatchIterator i = expression2.globalMatch(p);
            QStringList                     files;
            QStringList                     replace;


            while (i.hasNext() )
            {
                QRegularExpressionMatch match = i.next();
                QString                 f     = match.captured(1);


                replace << match.captured(0);
                files << f.replace("file:///", "");
            }

            // TODO: копирование картинок по пути
            for (int i = 0; i < replace.size(); i++)
            {
                p = p.replace(replace[i], "!(" + files[i] + ")");
            }


            // replace the indexes used in MS Word as
            // <span style=" vertical-align:sub;">1</span><span style=" vertical-align:super;">2</span>
            QString s1 = "", s2 = "", s3 = "";
            QString sub  = "vertical-align:sub;";
            QString sup  = "vertical-align:super;";
            QString span = "</span>";


            while (p.indexOf(sub) != -1)
            {
                s1 = p.left(p.indexOf(sub) );
                s1 = s1.left(s1.lastIndexOf("<") );
                s2 = p.mid(p.indexOf(sub) );
                s2 = s2.mid(s2.indexOf(">") + 1);
                s3 = s2.mid(s2.indexOf(span) + span.length() );
                s2 = s2.left(s2.indexOf(span) );
                p  = s1 + "[[sub]]" + s2 + "[[/sub]]" + s3;
            }

            while (p.indexOf(sup) != -1)
            {
                s1 = p.left(p.indexOf(sup) );
                s1 = s1.left(s1.lastIndexOf("<") );
                s2 = p.mid(p.indexOf(sup) );
                s2 = s2.mid(s2.indexOf(">") + 1);
                s3 = s2.mid(s2.indexOf(span) + span.length() );
                s2 = s2.left(s2.indexOf(span) );
                p  = s1 + "[[sup]]" + s2 + "[[/sup]]" + s3;
            }

            if (dlg.ui->bold_answer->isChecked() )
            {
                s2 = p.left(p.indexOf(">") );

                if (s2.indexOf("font-weight:") > -1)
                {
                    p = "*" + p;
                }
            }

            if (dlg.ui->color_answer->isChecked() )
            {
                /// background-color:transparent;
                QRegularExpression              expression3("background-color:(.*);");
                QRegularExpressionMatchIterator i       = expression3.globalMatch(p);
                bool                            colored = false;


                while (i.hasNext() )
                {
                    QRegularExpressionMatch match = i.next();


                    if (match.captured(1) != "transparent")
                    {
                        colored = true;
                    }
                }

                if (colored)
                {
                    p = "*" + p;
                }
            }

            p = p.remove(QRegExp("<[^>]*>") );

            if (p != "[[br]]")
            {
                lines << p;
            } else {
                lines << "";
            }
        }
        s = "\n" + lines.join("\n");

        // Удалить номер вопроса
        if (dlg.ui->repl_questnum->isChecked() )
        {
            QRegExp r("\\n\\s*[?]\\d+\\s*\\.\\s*");


            r.setMinimal(true);
            r.setCaseSensitivity(Qt::CaseInsensitive);
            s = s.replace(r, "\n?");
        }

        if (dlg.ui->repl_to_question->isChecked() )
        {
            // Заменить 1 на ?
            if (dlg.ui->question_marker->currentIndex() == 0)
            {
                QRegExp r("\\n\\s*\\d+\\s*");


                r.setMinimal(false);
                r.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r, "\n?");
            }

            // Заменить 1. на ?
            if (dlg.ui->question_marker->currentIndex() == 1)
            {
                QRegExp r("\\n\\s*\\d+\\s*\\.\\s*");


                r.setMinimal(true);
                r.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r, "\n?");
            }

            // Заменить 1) на ?
            if (dlg.ui->question_marker->currentIndex() == 2)
            {
                QRegExp r("\\n\\s*\\d+\\s*\\)\\s*");


                r.setMinimal(true);
                r.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r, "\n?");
            }
        }

        if (dlg.ui->repl_to_question_2->isChecked() )
        {
            QStringList lst = s.split("\n");


            for (auto &si : lst)
            {
                if (
                    si.startsWith("@")
                    || si.startsWith("#")
                    || si.startsWith("*")
                    || si.startsWith("?")
                    || si.startsWith("$") )
                {
                    continue;
                }

                // Удалить [а]
                if (dlg.ui->question_marker_2->currentIndex() == 4)
                {
                    QRegExp r("^\\s*\\[\\w\\]\\s*");


                    r.setMinimal(true);
                    r.setCaseSensitivity(Qt::CaseInsensitive);

                    if (si.contains(r) )
                    {
                        continue;
                    }


                    QRegExp r2("^\\s*\\*\\s*\\[\\w\\]\\s*");


                    r2.setMinimal(true);
                    r2.setCaseSensitivity(Qt::CaseInsensitive);

                    if (si.contains(r2) )
                    {
                        continue;
                    }
                }

                // Удалить а)
                if (dlg.ui->question_marker_2->currentIndex() == 3)
                {
                    QRegExp r("^\\s*\\w\\s*\\)\\s*");


                    r.setMinimal(true);
                    r.setCaseSensitivity(Qt::CaseInsensitive);

                    if (si.contains(r) )
                    {
                        continue;
                    }


                    QRegExp r2("^\\s*\\*\\s*\\w\\s*\\)\\s*");


                    r2.setMinimal(true);
                    r2.setCaseSensitivity(Qt::CaseInsensitive);

                    if (si.contains(r2) )
                    {
                        continue;
                    }
                }

                // Удалить А.
                if (dlg.ui->question_marker_2->currentIndex() == 2)
                {
                    QRegExp r("^\\s*\\w\\s*\\.\\s*");


                    r.setMinimal(true);
                    r.setCaseSensitivity(Qt::CaseInsensitive);

                    if (si.contains(r) )
                    {
                        continue;
                    }


                    QRegExp r2("^\\s*\\*\\s*\\w\\s*\\.\\s*");


                    r2.setMinimal(true);
                    r2.setCaseSensitivity(Qt::CaseInsensitive);

                    if (si.contains(r2) )
                    {
                        continue;
                    }
                }

                // Удалить 1.
                if (dlg.ui->question_marker_2->currentIndex() == 0)
                {
                    QRegExp r("^\\s*\\d+\\s*\\.\\s*");


                    r.setMinimal(true);
                    r.setCaseSensitivity(Qt::CaseInsensitive);

                    if (si.contains(r) )
                    {
                        continue;
                    }


                    QRegExp r2("^\\s*\\*\\s*\\d+\\s*\\.\\s*");


                    r2.setMinimal(true);
                    r2.setCaseSensitivity(Qt::CaseInsensitive);

                    if (si.contains(r2) )
                    {
                        continue;
                    }
                }

                // Удалить 1)
                if (dlg.ui->question_marker_2->currentIndex() == 1)
                {
                    QRegExp r("^\\d+\\s*\\)\\s*");


                    r.setMinimal(true);
                    r.setCaseSensitivity(Qt::CaseInsensitive);

                    if (si.contains(r) )
                    {
                        continue;
                    }


                    QRegExp r2("^\\s*\\*\\s*\\d+\\s*\\)\\s*");


                    r2.setMinimal(true);
                    r2.setCaseSensitivity(Qt::CaseInsensitive);

                    if (si.contains(r2) )
                    {
                        continue;
                    }
                }

                si = "?" + si;
            }

            s = lst.join("\n");
        }

        if (dlg.ui->empty_line->isChecked() )
        {
            // маркировать вопросом строку после пустой строки
            QRegExp r("\\n\\n+"); //[^\n]


            r.setMinimal(false);
            r.setCaseSensitivity(Qt::CaseInsensitive);
            s = s.replace(r, "\n?");
            s = "?" + s;

            if (dlg.ui->answer_marker->currentIndex() == 4)
            {
                QRegExp r("\\n\\s*\\[\\w\\]\\s*");


                r.setMinimal(true);
                r.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r, "\n");


                QRegExp r2("\\n\\s*\\*\\s*\\[\\w\\]\\s*");


                r2.setMinimal(true);
                r2.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r2, "\n*");
            }

            // Удалить а)
            if (dlg.ui->answer_marker->currentIndex() == 3)
            {
                QRegExp r("\\n\\s*\\w\\s*\\)\\s*");


                r.setMinimal(true);
                r.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r, "\n");


                QRegExp r2("\\n\\s*\\*\\s*\\w\\s*\\)\\s*");


                r2.setMinimal(true);
                r2.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r2, "\n*");
            }

            // Удалить А.
            if (dlg.ui->answer_marker->currentIndex() == 2)
            {
                QRegExp r("\\n\\s*\\w\\s*\\.\\s*");


                r.setMinimal(true);
                r.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r, "\n");


                QRegExp r2("\\n\\s*\\*\\s*\\w\\s*\\.\\s*");


                r2.setMinimal(true);
                r2.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r2, "\n*");
            }

            // Удалить 1.
            if (dlg.ui->answer_marker->currentIndex() == 0)
            {
                QRegExp r("\\n\\s*\\d+\\s*\\.\\s*");


                r.setMinimal(true);
                r.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r, "\n");


                QRegExp r2("\\n\\s*\\*\\s*\\d+\\s*\\.\\s*");


                r2.setMinimal(true);
                r2.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r2, "\n*");
            }

            // Удалить 1)
            if (dlg.ui->answer_marker->currentIndex() == 1)
            {
                QRegExp r("\\n\\d+\\s*\\)\\s*");


                r.setMinimal(true);
                r.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r, "\n");


                QRegExp r2("\\n\\s*\\*\\s*\\d+\\s*\\)\\s*");


                r2.setMinimal(true);
                r2.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r2, "\n*");
            }
        }

        if (dlg.ui->del_marker->isChecked() )
        {
            QString sCorrect   = "\n*";
            QString sInCorrect = "\n";


            if (dlg.ui->add_marker->isChecked() )
            {
                sCorrect   = "\n-*";
                sInCorrect = "\n-";
            }

            // Удалить [а]
            if (dlg.ui->answer_marker->currentIndex() == 4)
            {
                QRegExp r("\\n\\s*\\[\\w\\]\\s*");


                r.setMinimal(true);
                r.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r, sInCorrect);


                QRegExp r2("\\n\\s*\\*\\s*\\[\\w\\]\\s*");


                r2.setMinimal(true);
                r2.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r2, sCorrect);
            }

            // Удалить а)
            if (dlg.ui->answer_marker->currentIndex() == 3)
            {
                QRegExp r("\\n\\s*\\w\\s*\\)\\s*");


                r.setMinimal(true);
                r.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r, sInCorrect);


                QRegExp r2("\\n\\s*\\*\\s*\\w\\s*\\)\\s*");


                r2.setMinimal(true);
                r2.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r2, sCorrect);
            }

            // Удалить А.
            if (dlg.ui->answer_marker->currentIndex() == 2)
            {
                QRegExp r("\\n\\s*\\w\\s*\\.\\s*");


                r.setMinimal(true);
                r.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r, sInCorrect);


                QRegExp r2("\\n\\s*\\*\\s*\\w\\s*\\.\\s*");


                r2.setMinimal(true);
                r2.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r2, sCorrect);
            }

            // Удалить 1.
            if (dlg.ui->answer_marker->currentIndex() == 0)
            {
                QRegExp r("\\n\\s*\\d+\\s*\\.\\s*");


                r.setMinimal(true);
                r.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r, sInCorrect);


                QRegExp r2("\\n\\s*\\*\\s*\\d+\\s*\\.\\s*");


                r2.setMinimal(true);
                r2.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r2, sCorrect);
            }

            // Удалить 1)
            if (dlg.ui->answer_marker->currentIndex() == 1)
            {
                QRegExp r("\\n\\d+\\s*\\)\\s*");


                r.setMinimal(true);
                r.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r, sInCorrect);


                QRegExp r2("\\n\\s*\\*\\s*\\d+\\s*\\)\\s*");


                r2.setMinimal(true);
                r2.setCaseSensitivity(Qt::CaseInsensitive);
                s = s.replace(r2, sCorrect);
            }
        }

        // Удалить пробелы
        if (dlg.ui->del_space->isChecked() )
        {
            QRegExp r("\\n\\s+");


            r.setMinimal(false);
            r.setCaseSensitivity(Qt::CaseInsensitive);
            s = s.replace(r, "\n");


            QRegExp r1("\\s+\\n");


            r1.setMinimal(false);
            r1.setCaseSensitivity(Qt::CaseInsensitive);
            s = s.replace(r1, "\n");


            QRegExp r2("\\n\\*\\s+");


            r2.setMinimal(false);
            r2.setCaseSensitivity(Qt::CaseInsensitive);
            s = s.replace(r2, "\n*");


            QRegExp r3("\\n\\?\\s+");


            r3.setMinimal(false);
            r3.setCaseSensitivity(Qt::CaseInsensitive);
            s = s.replace(r3, "\n?");
        }

        // Коды вместо <>&&
        if (dlg.ui->repl_html->isChecked() )
        {
            s = s.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;");
        }

        // Заменить коды для <>&& на символы
        if (dlg.ui->make_tags->isChecked() )
        {
            s = s.replace("&lt;", "<").replace("&gt;", ">");
        }

        // Заменить
        if (dlg.ui->replfromto->isChecked() )
        {
            s = s.replace(dlg.ui->repl_from->text(), dlg.ui->repl_to->text() );
        }

        // разбивать строку по 1)
        if (dlg.ui->repl_split_numsk->isChecked() )
        {
            QRegExp r("\\s*\\d+\\s*\\)");


            r.setMinimal(true);
            r.setCaseSensitivity(Qt::CaseInsensitive);
            s = s.replace(r, "\n");
        }

        // разбивать строку по `а)`
        if (dlg.ui->repl_split_alpha->isChecked() )
        {
            QRegExp r("\\s*\\w\\s*\\)");


            r.setMinimal(true);
            r.setCaseSensitivity(Qt::CaseInsensitive);
            s = s.replace(r, "\n");
        }

        // Удалить `.` в конце строки
        if (dlg.ui->del_enddot->isChecked() )
        {
            s = s.replace(".\n", "\n");
        }

        // Удалить `,` в конце строки
        if (dlg.ui->del_endcomma->isChecked() )
        {
            s = s.replace(",\n", "\n");
        }

        // удалить `;` в конце строки
        if (dlg.ui->del_endsemicolon->isChecked() )
        {
            s = s.replace(";\n", "\n");
        }

        lines = s.split("\n");


        int     qnumber = 0;
        QString qformat;


        switch (dlg.ui->qnumber->currentIndex() )
        {
        case 1:
            {
                qformat = "?%1%2. ";
                break;
            }

        case 2:
            {
                qformat = "?%1%2) ";
                break;
            }

        default:
            {
                qformat = "?%1";
            }
        }

        for (int i = 0; i < lines.size(); i++)
        {
            // пронумеровать вопросы
            if (dlg.ui->qnumber->currentIndex() > 0)
            {
                // TODO: %%
                if (lines.at(i).leftRef(1) == "?")
                {
                    qnumber++;


                    QString w;
                    int     qpos = 1;


                    lines[i] = tr("?%1").arg(w).arg(qnumber) + lines.at(i).midRef(qpos);
                }

                if (
                    (lines.at(i).leftRef(1) == "$")
                    || (lines.at(i).leftRef(1) == "@")
                    || (lines.at(i).leftRef(1) == "#") )
                {
                    qnumber = 0;
                }
            }

            // признак вопроса
            if (dlg.ui->repl_to_quiz->isChecked() )
            {
                if (lines.at(i).mid(0, dlg.ui->label_quiz->text().length() ) == dlg.ui->label_quiz->text() )
                {
                    lines[i]     = "";
                    lines[i + 1] = "?" + lines[i + 1];
                }
            }

            // удалить в начале ответа
            if (dlg.ui->repl_answer->isChecked() )
            {
                if (lines.at(i).mid(0, dlg.ui->label_answer->text().length() ) == dlg.ui->label_answer->text() )
                {
                    lines[i] = lines[i].mid(dlg.ui->label_answer->text().length() );
                }
            }

            //признак ответа в конце
            if (dlg.ui->repl_to_correct_end->isChecked() )
            {
                if (lines.at(i).rightRef(dlg.ui->label_correct_end->text().length() ) ==
                    dlg.ui->label_correct_end->text() )
                {
                    lines[i] = "*" + lines[i].leftRef(lines[i].length() - dlg.ui->label_correct_end->text().length() );
                }
            }

            //признак ответа в конце
            if (dlg.ui->repl_to_correct_begin->isChecked() )
            {
                if (lines.at(i).leftRef(dlg.ui->label_correct_begin->text().length() ) ==
                    dlg.ui->label_correct_begin->text() )
                {
                    lines[i] =
                        "*" + lines[i].rightRef(lines[i].length() - dlg.ui->label_correct_begin->text().length() );
                }
            }
        }

        s = "<html><body><p>" + lines.join("</p><p>") + "</p></body></html>";
        ui->plain->setHtml(s);
    }
} // MainWindow::on_actionReplace_triggered


/// Collapse tree
void MainWindow::on_actionCollapse_triggered()
{
    QList<QTreeWidgetItem *> themes;
    QTreeWidgetItem          *top;

    for (int i = 0; i < ui->tree->topLevelItemCount(); i++)
    {
        top = ui->tree->topLevelItem(i);
        themes << top;

        for (int j = 0; j < top->childCount(); j++)
        {
            if (top->child(j)->text(0) == tr("theme") )
            {
                themes << top->child(j);
            }
        }
    }

    for (int i = 0; i < themes.size(); i++)
    {
        for (int j = 0; j < themes.at(i)->childCount(); j++)
        {
            top = themes.at(i)->child(j);

            if (top->text(0) != tr("theme") )
            {
                ui->tree->collapseItem(top);
            }
        }
    }
} // MainWindow::on_actionCollapse_triggered


/// Find in ui->plain
void MainWindow::on_actionFind_triggered()
{
    bool ok;

    if (ui->tree->isActiveWindow() )
    {
        if (ui->tree->selectedItems().size() > 0)
        {
            search = ui->tree->selectedItems().at(0)->text(1);
        }
    }


    QString s = QInputDialog::getText(this, tr("Find"), tr("Text"), QLineEdit::Normal, search, &ok);


    if (ok)
    {
        search = s;
        ui->plain->setTextCursor(ui->plain->document()->find(search) );
    }
} // MainWindow::on_actionFind_triggered


/// Find next in ui->plain
void MainWindow::on_actionFindNext_triggered()
{
    if (search.isEmpty() )
    {
        on_actionFind_triggered();
    } else {
        ui->plain->setTextCursor(ui->plain->document()->find(search, ui->plain->textCursor() ) );
    }
}


/// Edit theme or change question type
void MainWindow::on_tree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (item->childCount() != 0)
    {
        if ( (item->text(0) == tr("theme") ) || (item->text(0) == tr("section") ) )
        {
            change_section_name(item);
        } else if ( (item->text(0) == tr("text") ) || (item->text(0) == tr("number") )
                    || (item->text(0) == tr("multichoice") ) || (item->text(0) == tr("choice") )
                    || item->text(0) == tr("info") || item->text(0) == tr("essay") )
        {
            change_question_type(item);
        }

        // TODO: tr("correct") tr("incorrect")
    }
} // MainWindow::on_tree_itemDoubleClicked


/// Print of a position in a tree
void MainWindow::on_tree_itemSelectionChanged()
{
    QList<QTreeWidgetItem *> themes;
    QTreeWidgetItem          *top;
    QTreeWidgetItem          *selected = nullptr;

    if (ui->tree->selectedItems().size() > 0)
    {
        selected = ui->tree->selectedItems().at(0);

        if (selected->childCount() == 0)
        {
            selected = selected->parent();
        }
    }

    for (int i = 0; i < ui->tree->topLevelItemCount(); i++)
    {
        top = ui->tree->topLevelItem(i);
        themes << top;

        for (int j = 0; j < top->childCount(); j++)
        {
            if (top->child(j)->text(0) == tr("theme") )
            {
                themes << top->child(j);
            }
        }
    }


    int qcount = 0;
    int qid    = 0;


    for (int i = 0; i < themes.size(); i++)
    {
        for (int j = 0; j < themes.at(i)->childCount(); j++)
        {
            top = themes.at(i)->child(j);

            if (top->text(0) != tr("theme") )
            {
                qcount++;

                if (top == selected)
                {
                    qid = qcount;
                }
            }
        }
    }

    if (qid > 0)
    {
        treePositionLabel->setText(tr("Tree. %1 themes. %2 questions. Id %3").arg(themes.size() ).arg(qcount).arg(
                                       qid) );
    } else {
        treePositionLabel->setText(tr("Tree. %1 themes. %2 questions").arg(themes.size() ).arg(qcount) );
    }
} // MainWindow::on_tree_itemSelectionChanged


/// Change the type of all questions from shortanswer to numerical
void MainWindow::on_actionToNumerical_triggered()
{
    QList<QTreeWidgetItem *> themes;
    QTreeWidgetItem          *top;

    for (int i = 0; i < ui->tree->topLevelItemCount(); i++)
    {
        top = ui->tree->topLevelItem(i);
        themes << top;

        for (int j = 0; j < top->childCount(); j++)
        {
            if (top->child(j)->text(0) == tr("theme") )
            {
                themes << top->child(j);
            }
        }
    }

    for (int i = 0; i < themes.size(); i++)
    {
        for (int j = 0; j < themes.at(i)->childCount(); j++)
        {
            top = themes.at(i)->child(j);

            if (top->text(0) == tr("text") )
            {
                top->setText(0, tr("number") );
            }
        }
    }
} // MainWindow::on_actionToNumerical_triggered


void MainWindow::on_actionFromTickets_triggered()
{
    QList<QTreeWidgetItem *> themes;
    QTreeWidgetItem          *top;

    for (int i = 0; i < ui->tree->topLevelItemCount(); i++)
    {
        top = ui->tree->topLevelItem(i);
        themes << top;

        for (int j = 0; j < top->childCount(); j++)
        {
            if (top->child(j)->text(0) == tr("theme") )
            {
                themes << top->child(j);
            }
        }
    }


    QTreeWidgetItem                    *top_themes =
        new QTreeWidgetItem(QStringList() << tr("section") << tr("Themes") );
    QVector<QList<QTreeWidgetItem *> > new_themes;


    for (int i = 0; i < themes.size(); i++)
    {
        for (int j = 0; j < themes.at(i)->childCount(); j++)
        {
            top = themes.at(i)->child(j);

            if (top->text(0) == tr("theme") )
            {
                continue;
            }

            if (j >= new_themes.size() )
            {
                new_themes.resize(j + 1);
            }

            new_themes[j].append(top);
        }
    }

    ui->tree->addTopLevelItem(top_themes);

    for (int i = 0; i < new_themes.size(); i++)
    {
        top = new QTreeWidgetItem(QStringList() << tr("theme") << tr("Theme %1").arg(top_themes->childCount() + 1) );
        top_themes->addChild(top);

        for (int j = 0; j < new_themes.at(i).size(); j++)
        {
            new_themes.at(i)[j]->parent()->removeChild(new_themes.at(i)[j]);
            top->addChild(new_themes.at(i)[j]);
        }
    }
} // MainWindow::on_actionFromTickets_triggered


/// Change the type of error calculation for numerical
void MainWindow::on_actionFixedAccuracy_triggered(bool checked)
{
    // set default value of error calculation
    if (checked)
    {
        tolerance_string->setText(tr("Tolerance: ") );
        // fixed error
        tolerance->setText(tr("%1").arg(0.01) );
    } else {
        tolerance_string->setText(tr("Tolerance: 1/") );
        // denominator. error = value / demoninator
        tolerance->setText(tr("%1").arg(200) );
    }
}


void MainWindow::show_error(QTreeWidgetItem *item, QString message)
{
    ui->tree->collapseAll();
    ui->tree->clearSelection();
    // ui->tree->setItemSelected(item, true); // deprecated
    item->setSelected(true);
    ui->tree->expandItem(item);

    while (item->parent() )
    {
        item = item->parent();
        ui->tree->expandItem(item);
    }
    QMessageBox::warning(this, tr("Error"), message);
}


void MainWindow::on_actionHighlighter_triggered()
{
    //
    HighlighterDialog dlg;

    if (dlg.exec() )
    {
        dlg.highlighter->save_color();
        highlighter->load_color();
        highlighter->rehighlight();
    }
}


struct DSearch
{
    QString                    hash;
    QVector<QTreeWidgetItem *> question;
};


void MainWindow::on_actionRemoveNoAnswer_triggered()
{
    QList<QTreeWidgetItem *> themes;
    QTreeWidgetItem          *top;
    QTreeWidgetItem          *selected = nullptr;

    if (ui->tree->selectedItems().size() > 0)
    {
        selected = ui->tree->selectedItems().at(0);

        if (selected->childCount() == 0)
        {
            selected = selected->parent();
        }
    }

    for (int i = 0; i < ui->tree->topLevelItemCount(); i++)
    {
        top = ui->tree->topLevelItem(i);
        themes << top;

        for (int j = 0; j < top->childCount(); j++)
        {
            if (top->child(j)->text(0) == tr("theme") )
            {
                themes << top->child(j);
            }
        }
    }


    QVector<QTreeWidgetItem *> question;


    for (int i = 0; i < themes.size(); i++)
    {
        for (int j = 0; j < themes.at(i)->childCount(); j++)
        {
            top = themes.at(i)->child(j);

            if (top->text(0) != tr("theme") )
            {
                int correct = 0;


                for (int k = 0; k < top->childCount(); k++)
                {
                    if (top->child(k)->text(0) == tr("correct") )
                    {
                        correct++;
                    }
                }

                if (correct == 0)
                {
                    question << top;
                }
            }
        }
    }

    for (auto it : question)
    {
        it->parent()->removeChild(it);
    }
} // MainWindow::on_actionRemoveNoAnswer_triggered


void MainWindow::on_actionDuplicate_search_triggered()
{
    QMap<QString, DSearch>   cache;
    QList<QTreeWidgetItem *> themes;
    QTreeWidgetItem          *top, *item;
    QString                  hash;
    QStringList              strings;

    for (int i = 0; i < ui->tree->topLevelItemCount(); i++)
    {
        top = ui->tree->topLevelItem(i);
        themes << top;

        for (int j = 0; j < top->childCount(); j++)
        {
            if (top->child(j)->text(0) == tr("theme") )
            {
                themes << top->child(j);
            }
        }
    }

    for (int i = 0; i < themes.size(); i++)
    {
        for (int j = 0; j < themes.at(i)->childCount(); j++)
        {
            // TODO:
            item = themes.at(i)->child(j);

            if ( (item->text(0) == tr("theme") ) || (item->text(0) == tr("section") ) )
            {
                continue; // skip
            } else if (is_question(item) )
            {
                strings.clear();
                strings << item->text(1).replace(" ", "").toLower();

                for (int k = 0; k < item->childCount(); k++)
                {
                    if (item->child(k)->text(0) == tr("correct") )
                    {
                        strings << item->child(k)->text(1).replace(" ", "").toLower();
                    }
                }

                strings.sort();
                hash = strings.join("$");

                if (!cache.contains(hash) )
                {
                    cache[hash] =
                    {
                        hash
                    };
                }

                cache[hash].question << item;
            } else {
                continue;
                // TODO: other markers
            }
        }
    }

    ui->tree->collapseAll();

    for (DSearch &it : cache)
    {
        if (it.question.count() > 1)
        {
            for (auto i = 1; i < it.question.count(); i++)
            {
                it.question[i]->parent()->removeChild(
                    it.question[i]
                    );
                //                ui->tree->removeItemWidget(it.question[i]);
            }

            //            for (auto item : it.question)
            //            {
            //                ui->tree->expandItem(item);
            //
            //                while (item->parent() )
            //                {
            //                    item = item->parent();
            //                    ui->tree->expandItem(item);
            //                }
            //            }
        }
    }
} // MainWindow::on_actionDuplicate_search_triggered


void MainWindow::on_tree_customContextMenuRequested(const QPoint &pos)
{
    auto item = ui->tree->itemAt(pos);

    if (item)
    {
        QMenu *cnt = new QMenu();


        if (is_section(item) )
        {
            cnt->addAction(tr("Rename"), [item, this]()
            {
                change_section_name(item);
            });
        }

        if (is_question(item) )
        {
            cnt->addAction(tr("Change type"), [item, this]()
            {
                change_question_type(item);
            });
            cnt->addAction(tr("Edit"), [item, this]()
            {
                edit_question(item);
            });
            cnt->addAction(tr("Edit price"), [item, this]()
            {
                edit_price(item);
            });
            cnt->addAction(tr("Delete"), [item]()
            {
                item->parent()->removeChild(item);
                delete item;
            });
        }

        if (is_answer(item) )
        {
            cnt->addAction(tr("Edit"), [item, this]()
            {
                edit_question(item);
            });
            //            cnt->addAction(tr("Edit price"), [item, this]()
            //            {
            //                edit_price(item);
            //            });
            cnt->addAction(tr("Delete"), [item]()
            {
                item->parent()->removeChild(item);
                delete item;
            });
        }

        cnt->exec(ui->tree->mapToGlobal(pos) );
        delete cnt;
    }
} // MainWindow::on_tree_customContextMenuRequested


void MainWindow::change_section_name(QTreeWidgetItem *item)
{
    bool    ok;
    QString s =
        QInputDialog::getText(this, tr("Edit section"), tr("Section name"), QLineEdit::Normal, item->text(1), &ok);

    if (ok)
    {
        item->setText(1, s);
    }
}


void MainWindow::change_question_type(QTreeWidgetItem *item)
{
    bool        ok;
    QStringList items;

    if (item->text(0) == tr("map") )
    {
        QMessageBox::warning(this, tr("Error"), tr("Changing the type is not supported for this question") );

        return;
    }

    if (item->text(0) == tr("info") || item->text(0) == tr("essay") )
    {
        if (ui->format_essay->isChecked() && ui->format_info->isChecked() )
        {
            items << tr("info") << tr("essay");


            QString s = QInputDialog::getItem(
                this, tr("Change type"), tr("Question type"), items, items.indexOf(item->text(0) ), false, &ok);


            if (ok)
            {
                item->setText(0, s);
            }
        }
    } else {
        if (ui->format_choice->isChecked() )
        {
            items << tr("choice");
        }

        if (ui->format_multichoice->isChecked() )
        {
            items << tr("multichoice");
        }

        if (ui->format_shortanswer->isChecked() )
        {
            items << tr("text");
        }

        if (ui->format_numerical->isChecked() )
        {
            items << tr("number");
        }


        QString s = QInputDialog::getItem(
            this, tr("Change type"), tr("Question type"), items, items.indexOf(item->text(0) ), false, &ok);


        if (ok)
        {
            item->setText(0, s);
        }
    }
} // MainWindow::change_question_type


void MainWindow::edit_question(QTreeWidgetItem *item)
{
    if (is_question(item) )
    {
        TextDialog dlg;


        dlg.setText(item->text(1) );

        if (dlg.exec() )
        {
            item->setText(1, dlg.text() );
        }
    } else if (is_answer(item) )
    {
        if (item->parent()->text(0) == tr("map") || item->parent()->text(0) == tr("number") )
        {
            //TODO:
            QMessageBox::warning(this, tr("Error"), tr("In development") );
        } else {
            TextDialog dlg;


            dlg.setText(item->text(1) );

            if (dlg.exec() )
            {
                item->setText(1, dlg.text() );
            }
        }
    }
}


void MainWindow::edit_price(QTreeWidgetItem *item)
{
    bool ok;
    int  s =
        QInputDialog::getInt(this, tr("Edit price"), tr("Price"),
                             item->text(3).toInt(), -100, 100, 1, &ok);

    if (ok)
    {
        item->setText(3, tr("%1").arg(s) );
    }
}


void MainWindow::on_actionHelp_triggered()
{
    //help
    QDesktopServices::openUrl(QUrl::fromLocalFile(QApplication::applicationDirPath() + "/doc/manual.htm") );
}


void MainWindow::on_actionRequirements_triggered()
{
    //manual
    QDesktopServices::openUrl(QUrl::fromLocalFile(QApplication::applicationDirPath() + "/doc/manual.pdf") );
}


void MainWindow::on_actionMarkers_triggered()
{
    DialogConfig dlg;

    if (dlg.exec() )
    {
        //        highlighter->update_re();
    }
}
