#ifndef HIGHLIGHTERDIALOG_H
#define HIGHLIGHTERDIALOG_H

#include <QDialog>

namespace Ui
{

class HighlighterDialog;

}

class MyHighlighter;
class QTextCharFormat;

class HighlighterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HighlighterDialog(QWidget *parent = nullptr);
    ~HighlighterDialog();
    MyHighlighter *highlighter;

private slots:
    void on_comboBox_currentIndexChanged(int index);
    void flags_modify();
    void on_selectColor_clicked();

private:
    Ui::HighlighterDialog *ui;

    QTextCharFormat       *current;
};

#endif // HIGHLIGHTERDIALOG_H
