#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QRegExp>
#include <QTextStream>
#include <QTreeWidgetItem>
#include <dialogreplace.h>
#include <ui_dialogreplace.h>
#include "myhighlighter.h"
//#include <QtXml>

#define FORMAT_PLAIN 0
#define QUIZ_PER_FILE 10000
MainWindow::MainWindow(QWidget * parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    //    ui->toolBar->setVisible(false);
    sett = new QSettings("TSU", "TestConvert");
    last_dir = sett->value("dirs/last", "C:/work").toString();
    search = "";
    infolabel = new QLabel();
    ui->statusBar->addPermanentWidget(infolabel);
    tolerance = new QLineEdit("200");
    ui->toolBar->addWidget(tolerance);
    highlighter = new MyHighlighter(ui->plain->document());
}
MainWindow::~MainWindow() {
    sett->setValue("dirs/last", last_dir);
    delete infolabel;
    delete sett;
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *) {
    int w = ui->tree->width();

    ui->tree->setColumnWidth(0, w / 4);
    ui->tree->setColumnWidth(1, w / 2);
}
bool is_number(QString s) {
    s = s.replace(",", ".");
    bool ok;
    double v = s.toDouble(&ok);
    return ok;
}
QString to_number(QString s) { return s.replace(",", "."); }
void MainWindow::on_actionTest_triggered() {
    ui->tree->clear();

    QTreeWidgetItem *top, *child, *quest;

    top = new QTreeWidgetItem(QStringList() << tr("section") << tr("Course"));
    ui->tree->addTopLevelItem(top);

    int i = 0;
    QString s;
    QStringList data = ui->plain->document()->toPlainText().split("\n");

    while (i < data.size())
    {
        s = data.at(i);
        s = s.replace("&nbsp;", " ").replace("\t", "    ");

        while (s.length() > 0 && s[0] == ' ') { s = s.mid(1); }

        if (s == "")
        {
            i++;
            continue;
        }

        if (s.at(0) == '?')
        {
            quest = new QTreeWidgetItem(QStringList() << tr("unc") << s.mid(1));
            top->addChild(quest);
            i++;

            int j = i;
            QStringList ans;
            int correctcount = 0;

            while (j < data.size())
            {
                s = data.at(j);
                s = s.replace("&nbsp;", " ");

                while (s.length() > 0 && s[0] == ' ') { s = s.mid(1); }

                if (s == "")
                {
                    j++;
                    continue;
                }

                if (s.at(0) == '*') { correctcount++; }

                if ((s.at(0) == '#') || (s.at(0) == '@') || (s.at(0) == '?')) { break; }

                ans.append(s);
                j++;
            }

            i = j;

            if (ans.size() == 0)
            {
                quest->setText(0, tr("info"));
            } else if (ans.at(0).indexOf("->") > -1)
            {
                quest->setText(0, tr("map"));

                for (int k = 0; k < ans.size(); k++)
                {
                    s = ans.at(k);

                    if (s.at(0) == '*') { s = s.mid(1); }

                    quest->addChild(new QTreeWidgetItem(
                        QStringList() << tr("option") << s.mid(0, s.indexOf("->")) << s.mid(s.indexOf("->") + 2)));
                }
            } else
            {
                bool number = false;
                // TODO: все правильные галочками
                if ((correctcount == ans.size()) && (correctcount < 3))
                {
                    quest->setText(0, tr("text"));
                    number = true;
                    for (int k = 0; k < ans.size(); k++)
                    {
                        s = ans.at(k);
                        if (s.mid(0, 1) == "*") s = s.mid(1);
                        if (!is_number(s)) number = false;
                    }

                    if (number) { quest->setText(0, tr("number")); }
                } else if (correctcount > 1)
                {
                    quest->setText(0, tr("multichoice"));
                } else
                { quest->setText(0, tr("choice")); }

                for (int k = 0; k < ans.size(); k++)
                {
                    s = ans.at(k);

                    if (s.at(0) == '*')
                    {
                        if (number)
                        {
                            quest->addChild(new QTreeWidgetItem(QStringList() << tr("correct") << to_number(s.mid(1))));
                        } else
                        { quest->addChild(new QTreeWidgetItem(QStringList() << tr("correct") << s.mid(1))); }
                    } else
                    { quest->addChild(new QTreeWidgetItem(QStringList() << tr("incorrect") << s)); }
                }
            }

            continue;
        } else if (s.at(0) == '#')
        {
            top = new QTreeWidgetItem(QStringList() << tr("section") << s.mid(1));
            ui->tree->addTopLevelItem(top);
            i++;
            continue;
        } else if (s.at(0) == '@')
        {
            if (ui->tree->indexOfTopLevelItem(top) == -1) { top = top->parent(); }

            child = new QTreeWidgetItem(QStringList() << tr("theme") << s.mid(1));
            top->addChild(child);
            top = child;
            i++;
            continue;
        }

        i++;
    }

    ui->tree->expandAll();
    on_tree_itemSelectionChanged();
}    // MainWindow::on_actionTest_triggered

void MainWindow::on_actionOpen_triggered() {
    QString fname = QFileDialog::getOpenFileName(this, tr("Open"), last_dir, "html (*.htm;*.html);ANY FILE (*)");

    /*if (!fname.isEmpty())
    {
        images.clear();

        QFile f(fname);
        QFileInfo fi(fname);

        last_dir = fi.dir().path();
        imgsrcpath = last_dir;
        f.open(QIODevice::ReadOnly);

        QTextStream in(&f);

        in.setCodec("windows-1251");

        QString s = in.readAll();

        if (s.indexOf("text/html; charset=") > 0)
        {
            s = s.mid(s.indexOf("text/html; charset="), 50);
            s = s.mid(s.indexOf("=") + 1);
            s = s.left(s.indexOf("\""));
            in.setCodec(s.toStdString().c_str());
            in.seek(0);
            s = in.readAll();
        }


        QRegExp head("\\</head\\>");

        head.setCaseSensitivity(Qt::CaseInsensitive);

        int pos = head.indexIn(s);

        s = s.mid(pos);

        QRegExp math("\\<m:oMathPara\\>.*\\</m:oMathPara\\>");

        math.setMinimal(true);
        math.setCaseSensitivity(Qt::CaseInsensitive);
        s = s.replace(math, "");

        QRegExp re;

        re.setMinimal(true);
        re.setCaseSensitivity(Qt::CaseInsensitive);

        QRegExp re_ms;

        re_ms.setMinimal(true);
        re_ms.setCaseSensitivity(Qt::CaseInsensitive);

        QRegExp re2;

        re2.setMinimal(true);
        re2.setCaseSensitivity(Qt::CaseInsensitive);

        // обработка рисунков
        re.setPattern("\\<img.*\\>");
        re_ms.setPattern("\\<v:imagedata.*\\>");
        re2.setPattern("src\\s*=\\s*['\"](.*)['\"]");

        while (re.indexIn(s) > -1)
        {
            QString is = re.cap(0);

            re2.indexIn(is);

            QString url = re2.cap(1);

            imgsrcpath = last_dir + "/" + url.mid(0, url.indexOf("/"));

            while (url.indexOf("/") > -1) { url = url.mid(url.indexOf("/") + 1); }

            while (url.indexOf("\\") > -1) { url = url.mid(url.indexOf("\\") + 1); }

            images << url;
            s = s.replace(is, "!(" + url + ")");
        }

        // абзацы
        s = s.replace(QRegExp("[\r\n]"), " ");

        QRegExp p("\\<p\\s*.*\\>");

        p.setMinimal(true);
        p.setCaseSensitivity(Qt::CaseInsensitive);
        s = s.replace(p, "\n");

        QRegExp br("\\<br\\s*.*\\>");

        br.setMinimal(true);
        br.setCaseSensitivity(Qt::CaseInsensitive);
        s = s.replace(br, "&lt;br&gt;");

        // индексы
        QRegExp sub("\\<sub\\s*.*\\>");

        sub.setMinimal(true);
        sub.setCaseSensitivity(Qt::CaseInsensitive);
        s = s.replace(sub, "[sub]");

        QRegExp sup("\\<sup\\s*.*\\>");

        sup.setMinimal(true);
        sup.setCaseSensitivity(Qt::CaseInsensitive);
        s = s.replace(sup, "[sup]");

        QRegExp sub1("\\</sub\\s*.*\\>");

        sub1.setMinimal(true);
        sub1.setCaseSensitivity(Qt::CaseInsensitive);
        s = s.replace(sub1, "[/sub]");

        QRegExp sup1("\\</sup\\s*.*\\>");

        sup1.setMinimal(true);
        sup1.setCaseSensitivity(Qt::CaseInsensitive);
        s = s.replace(sup1, "[/sup]");

        // не воспринимаемые коды

        // все туги
        QRegExp notags("\\<.*\\>");

        notags.setMinimal(true);
        s = s.replace(notags, "");
        s = s.replace("-&gt;", "->");

        while (s.indexOf("  ") > -1) { s = s.replace("  ", " "); }

        s = s.replace("[sub]", "&lt;sub&gt;")
                .replace("[/sub]", "&lt;/sub&gt;")
                .replace("[sup]", "&lt;sup&gt;")
                .replace("[/sup]", "&lt;/sup&gt;");

        ui->plain->clear();
        ui->plain->appendPlainText(s);
    }*/
    // TODO: edit
}    // MainWindow::on_actionOpen_triggered

void MainWindow::writeText(QXmlStreamWriter & stream, QString txt, QString basepath) {
    QStringList images;
    while (txt.indexOf("!(") > -1)
    {
        QString s1 = txt.mid(0, txt.indexOf("!("));
        QString s2 = txt.mid(txt.indexOf("!(") + 2);
        QString s3 = s2.mid(s2.indexOf(")") + 1);

        s2 = s2.mid(0, s2.indexOf(")"));
        if (!images.contains(s2)) images.append(s2);
        txt = s1 + "<img src=\"@@PLUGINFILE@@/" + s2 + "\">" + s3;
    }

    stream.writeTextElement("text", txt);
    for (auto fn : images)
    {
        QFile file(basepath + "/" + fn);
        file.open(QIODevice::ReadOnly);

        stream.writeStartElement("file");
        stream.writeAttribute("name", fn);
        stream.writeAttribute("path", "/");
        stream.writeAttribute("encoding", "base64");
        stream.writeCharacters(file.readAll().toBase64());
        stream.writeEndElement();
    }
}
QString test_img(QString txt, QString imgpath) {
    txt = txt.replace("&", "&amp;");

    while (txt.indexOf("!(") > -1)
    {
        QString s1 = txt.mid(0, txt.indexOf("!("));
        QString s2 = txt.mid(txt.indexOf("!(") + 2);
        QString s3 = s2.mid(s2.indexOf(")") + 1);

        s2 = s2.mid(0, s2.indexOf(")"));
        txt = s1 + "&lt;img src=\"" + imgpath + s2 + "\"&gt;" + s3;
    }

    // замена недопустимых спецсимволов
    // txt = txt.replace("&cedil;", "...").replace("&times;", "&#215;");

    return txt;
}    // test_img

QString to_title(QString s) {
    QRegExp notags("\\<.*\\>");

    notags.setMinimal(true);
    s = s.replace(notags, "");

    QRegExp notags2("&lt;.*&gt;");

    notags2.setMinimal(true);
    s = s.replace(notags2, "");

    QRegExp nospec("&.*;");

    nospec.setMinimal(true);
    s = s.replace(nospec, "");

    return s;
}    // to_title

void MainWindow::on_actionExport_triggered() {
    double qtolerance, ktolerance;
    bool btolerance;
    btolerance = ui->actionFixTolerance->isChecked();
    bool ok;
    ktolerance = tolerance->text().toDouble(&ok);
    if (!ok)
    {
        QMessageBox::critical(this, tr("Error"), tr("Incorrect tolerance value"));
        return;
    }
    QString fname = QFileDialog::getSaveFileName(this, tr("Save"), last_dir + "/quiz.xml", tr("xml-file (*.xml)"));
    int qmax = QUIZ_PER_FILE;

    if (!fname.isEmpty())
    {
        /*
         * Настройка
         */
        QString open_xml;
        QString close_xml;
        QString text;
        QString number;
        QString multichoice;
        QString choice;
        QString theme;
        QString section;
        QString map;
        QString info;
        QString answer;
        QString numanswer;
        QString mapanswer;


        // -------------------------------
        QFileInfo fi(fname);

        last_dir = fi.dir().path();

        int fnum = 0;
        int qcount = 0;
        bool opened = false;
        bool as_multi = ui->ch_to_mch->isChecked();    // false; //экспорт choice как multichoice
        bool as_text = ui->num_to_text->isChecked();    // false; //экспорт number как text

        QList<QTreeWidgetItem *> themes;

        QTreeWidgetItem * top;

        for (int i = 0; i < ui->tree->topLevelItemCount(); i++)
        {
            top = ui->tree->topLevelItem(i);
            themes << top;

            for (int j = 0; j < top->childCount(); j++)
            {
                if (top->child(j)->text(0) == tr("theme")) { themes << top->child(j); }
            }
        }

        QFile f;
        f.setFileName(fname);
        f.open(QIODevice::WriteOnly);
        QXmlStreamWriter stream(&f);
        stream.setAutoFormatting(true);
        stream.writeStartDocument();

        stream.writeStartElement("quiz");
        //        stream.writeAttribute("href", "http://qt-project.org/");
        //        stream.writeTextElement("title", "Qt Project");
        //        stream.writeEndElement();    // bookmark
        for (int i = 0; i < themes.size(); i++)
        {
            if (themes.at(i)->text(0) == tr("theme"))
            {
                stream.writeStartElement("question");
                stream.writeAttribute("type", "category");
                stream.writeStartElement("category");
                stream.writeTextElement("text", themes.at(i)->parent()->text(1) + "/" + themes.at(i)->text(1));
                stream.writeEndElement();
                stream.writeEndElement();
            } else
            {
                stream.writeStartElement("question");
                stream.writeAttribute("type", "category");
                stream.writeStartElement("category");
                stream.writeTextElement("text", themes.at(i)->text(1));
                stream.writeEndElement();
                stream.writeEndElement();
            }
            for (int j = 0; j < themes.at(i)->childCount(); j++)
            {
                top = themes.at(i)->child(j);

                if (top->text(0) == tr("theme")) { continue; }

                QString ans = "";

                if (top->text(0) == tr("info"))
                {
                    stream.writeStartElement("question");
                    stream.writeAttribute("type", "description");
                    stream.writeStartElement("name");
                    stream.writeTextElement("text", to_title(top->text(1)));
                    stream.writeEndElement();
                    stream.writeStartElement("questiontext");
                    stream.writeAttribute("format", "moodle_auto_format");
                    //                    stream.writeTextElement("text", top->text(1));
                    writeText(stream, top->text(1), last_dir);
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
                } else if (top->text(0) == tr("map"))
                {
                    stream.writeStartElement("question");
                    stream.writeAttribute("type", "matching");
                    stream.writeStartElement("name");
                    stream.writeTextElement("text", to_title(top->text(1)));
                    stream.writeEndElement();
                    stream.writeStartElement("questiontext");
                    stream.writeAttribute("format", "moodle_auto_format");
                    writeText(stream, top->text(1), last_dir);
                    // add image
                    stream.writeEndElement();
                    stream.writeStartElement("generalfeedback");
                    stream.writeTextElement("text", "");
                    stream.writeEndElement();
                    stream.writeTextElement("defaultgrade", "10");
                    stream.writeTextElement("penalty", "1");
                    stream.writeTextElement("hidden", "0");
                    stream.writeTextElement("shuffleanswers", "1");
                    // answ
                    for (int k = 0; k < top->childCount(); k++)
                    {
                        if (top->child(k)->text(0) == tr("option"))
                        {
                            stream.writeStartElement("subquestion");
                            writeText(stream, top->child(k)->text(1), last_dir);
                            stream.writeStartElement("answer");
                            stream.writeTextElement("text", top->child(k)->text(2));
                            stream.writeEndElement();
                            stream.writeEndElement();
                        }
                    }
                    stream.writeEndElement();
                } else if ((top->text(0) == tr("text")) || ((top->text(0) == tr("number")) && as_text))
                {
                    stream.writeStartElement("question");
                    stream.writeAttribute("type", "shortanswer");
                    stream.writeStartElement("name");
                    stream.writeTextElement("text", to_title(top->text(1)));
                    stream.writeEndElement();
                    stream.writeStartElement("questiontext");
                    stream.writeAttribute("format", "moodle_auto_format");
                    writeText(stream, top->text(1), last_dir);
                    // add image
                    stream.writeEndElement();
                    stream.writeStartElement("generalfeedback");
                    stream.writeTextElement("text", "");
                    stream.writeEndElement();
                    stream.writeTextElement("defaultgrade", "10");
                    stream.writeTextElement("penalty", "1");
                    stream.writeTextElement("hidden", "0");
                    stream.writeTextElement("shuffleanswers", "1");
                    // answ
                    for (int k = 0; k < top->childCount(); k++)
                    {
                        if (top->child(k)->text(0) == tr("correct"))
                        {
                            stream.writeStartElement("answer");
                            stream.writeAttribute("fraction", "100");
                            stream.writeTextElement("text", top->child(k)->text(1));
                            stream.writeStartElement("feedback");
                            stream.writeTextElement("text", "");
                            stream.writeEndElement();
                            stream.writeEndElement();
                        }
                    }

                    stream.writeEndElement();
                } else if (top->text(0) == tr("number"))
                {
                    stream.writeStartElement("question");
                    stream.writeAttribute("type", "numerical");
                    stream.writeStartElement("name");
                    stream.writeTextElement("text", to_title(top->text(1)));
                    stream.writeEndElement();
                    stream.writeStartElement("questiontext");
                    stream.writeAttribute("format", "moodle_auto_format");
                    writeText(stream, top->text(1), last_dir);
                    // add image
                    stream.writeEndElement();
                    stream.writeStartElement("generalfeedback");
                    stream.writeTextElement("text", "");
                    stream.writeEndElement();
                    stream.writeTextElement("defaultgrade", "10");
                    stream.writeTextElement("penalty", "1");
                    stream.writeTextElement("hidden", "0");
                    stream.writeTextElement("shuffleanswers", "1");
                    // answ
                    for (int k = 0; k < top->childCount(); k++)
                    {
                        if (top->child(k)->text(0) == tr("correct"))
                        {
                            stream.writeStartElement("answer");
                            stream.writeAttribute("fraction", "100");
                            if (btolerance)
                                qtolerance = ktolerance;
                            else
                                qtolerance = abs(top->child(k)->text(1).toDouble() / ktolerance);
                            stream.writeAttribute("tolerance", QString("%1").arg(qtolerance));
                            stream.writeTextElement("text", top->child(k)->text(1));
                            stream.writeStartElement("feedback");
                            stream.writeTextElement("text", "");
                            stream.writeEndElement();
                            stream.writeEndElement();
                        }
                    }
                    stream.writeEndElement();
                } else if ((top->text(0) == tr("multichoice")) || ((top->text(0) == tr("choice")) && as_multi))
                {
                    stream.writeStartElement("question");
                    stream.writeAttribute("type", "multichoice");
                    stream.writeStartElement("name");
                    stream.writeTextElement("text", to_title(top->text(1)));
                    stream.writeEndElement();
                    stream.writeStartElement("questiontext");
                    stream.writeAttribute("format", "moodle_auto_format");
                    writeText(stream, top->text(1), last_dir);
                    // add image
                    stream.writeEndElement();
                    stream.writeStartElement("generalfeedback");
                    stream.writeTextElement("text", "");
                    stream.writeEndElement();
                    stream.writeTextElement("single", "false");
                    stream.writeTextElement("defaultgrade", "10");
                    stream.writeTextElement("penalty", "1");
                    stream.writeTextElement("hidden", "0");
                    stream.writeTextElement("shuffleanswers", "1");
                    // answ
                    int correct = 0, incorrect = 0;

                    for (int k = 0; k < top->childCount(); k++)
                    {
                        if (top->child(k)->text(0) == tr("correct"))
                        {
                            correct++;
                        } else
                        { incorrect++; }
                    }
                    if ((correct == 0) || ((incorrect == 0) && !ui->action_enableAll->isChecked()))
                    {
                        ui->tree->collapseAll();
                        ui->tree->expandItem(top->parent()->parent());
                        ui->tree->expandItem(top->parent());
                        ui->tree->expandItem(top);
                        QMessageBox::warning(this, tr("Error"), tr("Answer (correct==0)||(incorrect==0)"));

                        return;
                    }
                    for (int k = 0; k < top->childCount(); k++)
                    {
                        stream.writeStartElement("answer");
                        if (top->child(k)->text(0) == tr("correct"))
                        {
                            stream.writeAttribute("fraction", QString("%1").arg(100.0 / correct));

                        } else
                        { stream.writeAttribute("fraction", QString("%1").arg(-100.0 / incorrect)); }
                        writeText(stream, top->child(k)->text(1), last_dir);
                        stream.writeStartElement("feedback");
                        stream.writeTextElement("text", "");
                        stream.writeEndElement();
                        stream.writeEndElement();
                    }

                    stream.writeEndElement();
                } else if (top->text(0) == tr("choice"))
                {
                    stream.writeStartElement("question");
                    stream.writeAttribute("type", "multichoice");
                    stream.writeStartElement("name");
                    stream.writeTextElement("text", to_title(top->text(1)));
                    stream.writeEndElement();
                    stream.writeStartElement("questiontext");
                    stream.writeAttribute("format", "moodle_auto_format");
                    writeText(stream, top->text(1), last_dir);
                    // add image
                    stream.writeEndElement();
                    stream.writeStartElement("generalfeedback");
                    stream.writeTextElement("text", "");
                    stream.writeEndElement();
                    stream.writeTextElement("single", "true");
                    stream.writeTextElement("defaultgrade", "10");
                    stream.writeTextElement("penalty", "1");
                    stream.writeTextElement("hidden", "0");
                    stream.writeTextElement("shuffleanswers", "1");
                    // answ

                    int correct = 0;
                    for (int k = 0; k < top->childCount(); k++)
                    {
                        stream.writeStartElement("answer");
                        if (top->child(k)->text(0) == tr("correct"))
                        {
                            stream.writeAttribute("fraction", "100");
                            correct++;

                        } else
                        { stream.writeAttribute("fraction", "0"); }

                        writeText(stream, top->child(k)->text(1), last_dir);
                        stream.writeStartElement("feedback");
                        stream.writeTextElement("text", "");
                        stream.writeEndElement();
                        stream.writeEndElement();
                    }
                    if (correct == 0)
                    {
                        ui->tree->collapseAll();
                        ui->tree->expandItem(top->parent()->parent());
                        ui->tree->expandItem(top->parent());
                        ui->tree->expandItem(top);

                        QMessageBox::warning(this, tr("Error"), tr("No correct answer"));

                        return;
                    }
                    stream.writeEndElement();
                }
            }
        }
        stream.writeEndElement();    // bookmark
        stream.writeEndDocument();
        f.close();
    }
}    // MainWindow::on_actionExport_triggered

void MainWindow::on_action_repl_triggered() {
    DialogReplace dlg;
    //    TODO: edit

    if (dlg.exec())
    {
        // списки замен
        QString s = ui->plain->document()->toHtml();
        QStringList lines;
        int pos = s.indexOf("<body", 0, Qt::CaseInsensitive);
        s = s.mid(pos);
        s = s.mid(s.indexOf(">") + 1);
        s = s.left(s.indexOf("</body", 0, Qt::CaseInsensitive));
        // TODO: H tag
        while (true)
        {
            if (s.isEmpty()) break;
            pos = s.indexOf("</p>", 0, Qt::CaseInsensitive);

            QString p = s.left(pos);
            s = s.mid(pos + 4);

            p = p.mid(p.indexOf(">") + 1);
            p = p.replace("<br />", "[[br]]")
                    .replace("<sub>", "[[sub]]")
                    .replace("</sub>", "[[/sub]]")
                    .replace("<sup>", "[[sup]]")
                    .replace("</sup>", "[[/sup]]");
            qWarning(p.toStdString().c_str());
            qWarning("----");
        }
        /*
        // Удалить номер вопроса
        if (dlg.ui->repl_questnum->isChecked())
        {
            qWarning("repl_questnum");

            QRegExp r("\\n\\s*[?]\\d+\\s*\\.\\s*");

            r.setMinimal(true);
            r.setCaseSensitivity(Qt::CaseInsensitive);
            s = s.replace(r, "\n?");
        }
        // Заменить 1. на ?
        if (dlg.ui->repl_numdot->isChecked())
        {
            QRegExp r("\\n\\s*\\d+\\s*\\.\\s*");

            r.setMinimal(true);
            r.setCaseSensitivity(Qt::CaseInsensitive);
            s = s.replace(r, "\n?");
        }
        // Заменить 1 на ?
        if (dlg.ui->repl_num->isChecked())
        {
            QRegExp r("\\n\\s*\\d+\\s*");

            r.setMinimal(false);
            r.setCaseSensitivity(Qt::CaseInsensitive);
            s = s.replace(r, "\n?");
        }
        // Заменить 1) на ?
        if (dlg.ui->repl_numsk->isChecked())
        {
            QRegExp r("\\n\\s*\\d+\\s*\\)\\s*");

            r.setMinimal(true);
            r.setCaseSensitivity(Qt::CaseInsensitive);
            s = s.replace(r, "\n?");
        }
        // Удалить а)
        if (dlg.ui->del_alpha->isChecked())
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
        if (dlg.ui->del_alphadot->isChecked())
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
        if (dlg.ui->del_num->isChecked())
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
        if (dlg.ui->del_numsk->isChecked())
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
        // Удалить пробелы
        if (dlg.ui->del_space->isChecked())
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
        if (dlg.ui->repl_html->isChecked()) { s = s.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;"); }
        // Заменить коды для <>&& на символы
        if (dlg.ui->make_tags->isChecked()) { s = s.replace("&lt;", "<").replace("&gt;", ">"); }
        // Заменить
        if (dlg.ui->replfromto->isChecked()) { s = s.replace(dlg.ui->repl_from->text(), dlg.ui->repl_to->text()); }
        // разбивать строку по 1)
        if (dlg.ui->repl_split_numsk->isChecked())
        {
            QRegExp r("\\s*\\d+\\s*\\)");

            r.setMinimal(true);
            r.setCaseSensitivity(Qt::CaseInsensitive);
            s = s.replace(r, "\n");
        }
        // разбивать строку по а)
        if (dlg.ui->repl_split_alpha->isChecked())
        {
            QRegExp r("\\s*\\w\\s*\\)");

            r.setMinimal(true);
            r.setCaseSensitivity(Qt::CaseInsensitive);
            s = s.replace(r, "\n");
        }
        // Удалить . в конце строки
        if (dlg.ui->del_enddot->isChecked()) { s = s.replace(".\n", "\n"); }
        // Удалить , в конце строки
        if (dlg.ui->del_endcomma->isChecked()) { s = s.replace(",\n", "\n"); }
        // удалить ; в конце строки
        if (dlg.ui->del_endsemicolon->isChecked()) { s = s.replace(";\n", "\n"); }

        QStringList lst = s.split("\n");

        for (int i = 0; i < lst.size(); i++)
        {
            // признак вопроса
            if (dlg.ui->repl_to_quiz->isChecked())
            {
                if (lst.at(i).mid(0, dlg.ui->label_quiz->text().length()) == dlg.ui->label_quiz->text())
                {
                    lst[i] = "";
                    lst[i + 1] = "?" + lst[i + 1];
                }
            }
            // удалить в начале ответа
            if (dlg.ui->repl_answer->isChecked())
            {
                if (lst.at(i).mid(0, dlg.ui->label_answer->text().length()) == dlg.ui->label_answer->text())
                { lst[i] = lst[i].mid(dlg.ui->label_answer->text().length()); }
            }
            //признак ответа в конце
            if (dlg.ui->repl_to_correct->isChecked())
            {
                if (lst.at(i).rightRef(dlg.ui->label_correct->text().length()) == dlg.ui->label_correct->text())
                { lst[i] = "*" + lst[i].left(lst[i].length() - dlg.ui->label_correct->text().length()); }
            }         
        }

        ui->plain->clear();
        ui->plain->setPlainText(lst.join("\n"));*/
    }
}    // MainWindow::on_action_repl_triggered

void MainWindow::on_collapse_quiz_triggered() {
    QList<QTreeWidgetItem *> themes;

    QTreeWidgetItem * top;

    for (int i = 0; i < ui->tree->topLevelItemCount(); i++)
    {
        top = ui->tree->topLevelItem(i);
        themes << top;

        for (int j = 0; j < top->childCount(); j++)
        {
            if (top->child(j)->text(0) == tr("theme")) { themes << top->child(j); }
        }
    }

    for (int i = 0; i < themes.size(); i++)
    {
        for (int j = 0; j < themes.at(i)->childCount(); j++)
        {
            top = themes.at(i)->child(j);

            if (top->text(0) != tr("theme")) { ui->tree->collapseItem(top); }
        }
    }
}    // MainWindow::on_collapse_quiz_triggered

void MainWindow::on_action_find_triggered() {
    bool ok;

    if (ui->tree->isActiveWindow())
    {
        if (ui->tree->selectedItems().size() > 0) { search = ui->tree->selectedItems().at(0)->text(1); }
    }

    QString s = QInputDialog::getText(this, tr("Find"), tr("Text"), QLineEdit::Normal, search, &ok);

    if (ok)
    {
        search = s;
        ui->plain->setTextCursor(ui->plain->document()->find(search));
    }
}    // MainWindow::on_action_find_triggered

void MainWindow::on_action_next_triggered() {
    if (search.isEmpty())
    {
        on_action_find_triggered();
    } else
    { ui->plain->setTextCursor(ui->plain->document()->find(search, ui->plain->textCursor())); }
}

void MainWindow::on_tree_itemDoubleClicked(QTreeWidgetItem * item, int column) {
    if (item->childCount() != 0)
    {
        if ((item->text(0) == tr("theme")) || (item->text(0) == tr("section")))
        {
            bool ok;
            QString s =
                QInputDialog::getText(this, tr("Edit theme"), tr("Theme name"), QLineEdit::Normal, item->text(1), &ok);

            if (ok) { item->setText(1, s); }
        } else if ((item->text(0) == tr("text")) || (item->text(0) == tr("number")) ||
                   (item->text(0) == tr("multichoice")) || (item->text(0) == tr("choice")))
        {
            bool ok;
            QStringList items;

            items << tr("text") << tr("number") << tr("multichoice") << tr("choice");

            QString s = QInputDialog::getItem(
                this, tr("Change type"), tr("Question type"), items, items.indexOf(item->text(0)), false, &ok);

            if (ok) { item->setText(0, s); }
        }
    }
}    // MainWindow::on_tree_itemDoubleClicked

void MainWindow::on_tree_itemSelectionChanged() {
    QList<QTreeWidgetItem *> themes;

    QTreeWidgetItem * top;
    QTreeWidgetItem * selected;

    if (ui->tree->selectedItems().size() > 0)
    {
        selected = ui->tree->selectedItems().at(0);

        if (selected->childCount() == 0) { selected = selected->parent(); }
    }

    for (int i = 0; i < ui->tree->topLevelItemCount(); i++)
    {
        top = ui->tree->topLevelItem(i);
        themes << top;

        for (int j = 0; j < top->childCount(); j++)
        {
            if (top->child(j)->text(0) == tr("theme")) { themes << top->child(j); }
        }
    }

    int qcount = 0;
    int qid = 0;

    for (int i = 0; i < themes.size(); i++)
    {
        for (int j = 0; j < themes.at(i)->childCount(); j++)
        {
            top = themes.at(i)->child(j);

            if (top->text(0) != tr("theme"))
            {
                qcount++;

                if (top == selected) { qid = qcount; }
            }
        }
    }

    if (qid > 0)
    {
        infolabel->setText(tr("Tree. %1 themes. %2 questions. Id %3").arg(themes.size()).arg(qcount).arg(qid));
    } else
    { infolabel->setText(tr("Tree. %1 themes. %2 questions").arg(themes.size()).arg(qcount)); }
}    // MainWindow::on_tree_itemSelectionChanged

void MainWindow::on_action_textToNum_triggered() {
    QList<QTreeWidgetItem *> themes;

    QTreeWidgetItem * top;

    for (int i = 0; i < ui->tree->topLevelItemCount(); i++)
    {
        top = ui->tree->topLevelItem(i);
        themes << top;

        for (int j = 0; j < top->childCount(); j++)
        {
            if (top->child(j)->text(0) == tr("theme")) { themes << top->child(j); }
        }
    }

    for (int i = 0; i < themes.size(); i++)
    {
        for (int j = 0; j < themes.at(i)->childCount(); j++)
        {
            top = themes.at(i)->child(j);

            if (top->text(0) == tr("text")) { top->setText(0, tr("number")); }
        }
    }
}    // MainWindow::on_action_textToNum_triggered

void MainWindow::on_action_fromBilet_triggered() {
    QList<QTreeWidgetItem *> themes;

    QTreeWidgetItem * top;

    for (int i = 0; i < ui->tree->topLevelItemCount(); i++)
    {
        top = ui->tree->topLevelItem(i);
        themes << top;

        for (int j = 0; j < top->childCount(); j++)
        {
            if (top->child(j)->text(0) == tr("theme")) { themes << top->child(j); }
        }
    }

    QTreeWidgetItem * top_themes = new QTreeWidgetItem(QStringList() << tr("section") << tr("Themes"));

    QVector<QList<QTreeWidgetItem *>> new_themes;

    for (int i = 0; i < themes.size(); i++)
    {
        for (int j = 0; j < themes.at(i)->childCount(); j++)
        {
            top = themes.at(i)->child(j);

            if (top->text(0) == tr("theme")) { continue; }

            if (j >= new_themes.size()) { new_themes.resize(j + 1); }

            new_themes[j].append(top);
        }
    }

    ui->tree->addTopLevelItem(top_themes);

    for (int i = 0; i < new_themes.size(); i++)
    {
        top = new QTreeWidgetItem(QStringList() << tr("theme") << tr("Theme %1").arg(top_themes->childCount() + 1));
        top_themes->addChild(top);

        for (int j = 0; j < new_themes.at(i).size(); j++)
        {
            new_themes.at(i)[j]->parent()->removeChild(new_themes.at(i)[j]);
            top->addChild(new_themes.at(i)[j]);
        }
    }
}    // MainWindow::on_action_fromBilet_triggered

void MainWindow::on_actionFixTolerance_triggered(bool checked) {
    //
    if (checked)
        tolerance->setText(tr("%1").arg(0.01));
    else
        tolerance->setText(tr("%1").arg(200));
}
/*
void MainWindow::on_action_triggered() {
    //    QClipboard * clipboard = QGuiApplication::clipboard();
    //    const QMimeData * mimeData = clipboard->mimeData();
    //    qWarning(mimeData->formats().join("\n").toStdString().c_str());
    //    //    QString originalText = clipboard->text();
    //    QString originalText = mimeData->html();
    //    qWarning(originalText.toStdString().c_str());
    QString s = ui->textEdit->toHtml();
    QRegularExpression expression2("src\\s*=\\s*\"(.+?)\"");
    QRegularExpressionMatchIterator i = expression2.globalMatch(s);
    QStringList files;
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        QString s = match.captured(1);
        if (s.indexOf("file:///") == 0) files << s;
    }
    QString path = QString::number(QDateTime::currentSecsSinceEpoch(), 16);
    QDir work = QDir::current();
    work.mkpath(path);
    path = work.relativeFilePath(path);
    int cnt = 0;
    for (auto filename : files)
    {
        QFileInfo f(filename.mid(8));
        QString newname = path + "/" + f.fileName().toStdString().c_str();
        if (QFile::copy(f.filePath().toStdString().c_str(), newname))
        {
            cnt++;
            s = s.replace(filename, newname);
        }
    }
    ui->textEdit->setHtml(s);
    QMessageBox::information(this, tr("Done"), tr("Copied %1 files").arg(cnt));
    if (s.indexOf("font-family:'Symbol'") != -1)
    { QMessageBox::warning(this, tr("Warning"), tr("The Symbol font suspicion")); }
}
*/
