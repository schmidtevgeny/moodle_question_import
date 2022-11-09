#include "highlighterdialog.h"
#include "myhighlighter.h"
#include "ui_highlighterdialog.h"
#include <QColorDialog>

HighlighterDialog::HighlighterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HighlighterDialog)
{
    ui->setupUi(this);
    highlighter = new MyHighlighter(ui->plain->document() );
    on_comboBox_currentIndexChanged(0);
}

HighlighterDialog::~HighlighterDialog()
{
    delete ui;
}


void HighlighterDialog::on_comboBox_currentIndexChanged(int index)
{
    // save/load color
    switch (ui->comboBox->currentIndex() )
    {
    case 0:
        {
            current = &highlighter->keywordFormat;
            break;
        }

    case 1:
        {
            current = &highlighter->sectionFormat;
            break;
        }

    case 2:
        {
            current = &highlighter->subsectionFormat;
            break;
        }

    case 3:
        {
            current = &highlighter->ticketFormat;
            break;
        }

    case 4:
        {
            current = &highlighter->questionFormat;
            break;
        }

    case 5:
        {
            current = &highlighter->correctAnswerFormat;
            break;
        }

    case 6:
        {
            current = &highlighter->incorrectAnswerFormat;
            break;
        }

    case 7:
        {
            current = &highlighter->priceFormat;
            break;
        }

    default:
        {
            qWarning("unknown index");
        }
    } // switch

    ui->isItalic->setChecked(current->fontItalic() );
    ui->isUnderline->setChecked(current->fontUnderline() );
    ui->isBold->setChecked( (current->fontWeight() == QFont::Bold) );
    ui->selectColor->setStyleSheet("background-color:" + current->foreground().color().name() );
} // HighlighterDialog::on_comboBox_currentIndexChanged


void HighlighterDialog::flags_modify()
{
    current->setFontItalic(ui->isItalic->isChecked() );
    current->setFontUnderline(ui->isUnderline->isChecked() );

    if (ui->isBold->isChecked() )
    {
        current->setFontWeight(QFont::Bold);
    } else {
        current->setFontWeight(QFont::Normal);
    }

    highlighter->rehighlight();
}


void HighlighterDialog::on_selectColor_clicked()
{
    QColor cl = QColorDialog::getColor(current->foreground().color() );


    ui->selectColor->setStyleSheet("background-color:" + cl.name() );
    current->setForeground(cl);
    highlighter->rehighlight();
}
