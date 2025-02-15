#include "myhighlighter.h"
#include "QSettings"
#include <QRegularExpression>
// TODO: make normal variables
MyHighlighter::MyHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
    , sectionExpression("^\\s*(#)[^\\n]*")
    , ticketExpression("^\\s*(\\$)[^\\n]*")
    , correctAnswerExpression("^\\s*(\\*)(\\d*.?\\d*%%)?[^\\n]*")
    , incorrectAnswerExpression("^\\s*()(-\\d*.?\\d*%%)?[^#@\\?\\*$][^\\n]*")
    , questionExpression("^\\s*(\\?(!)?)(\\d*.?\\d*%%)?[^\\n]*")
    , subsectionExpression("^\\s*(@)[^\\n]*")
    , matchExpression("^\\s*\\*[^\\n]*(->)", QRegularExpression::InvertedGreedinessOption)
{
    keywordFormat.setFontWeight(QFont::Bold);
    sectionFormat.setFontWeight(QFont::Bold);
    ticketFormat.setFontWeight(QFont::Bold);
    subsectionFormat.setFontWeight(QFont::Bold);
    questionFormat.setFontWeight(QFont::Bold);
    correctAnswerFormat.setFontWeight(QFont::Normal);
    incorrectAnswerFormat.setFontWeight(QFont::Normal);
    priceFormat.setFontWeight(QFont::Normal);

    keywordFormat.setForeground(Qt::red);
    sectionFormat.setForeground(Qt::black);
    ticketFormat.setForeground(Qt::black);
    subsectionFormat.setForeground(Qt::black);
    questionFormat.setForeground(Qt::black);
    correctAnswerFormat.setForeground(Qt::darkGreen);
    incorrectAnswerFormat.setForeground(Qt::darkMagenta);
    priceFormat.setForeground(Qt::green);

    keywordFormat.setFontItalic(false);
    sectionFormat.setFontItalic(true);
    ticketFormat.setFontItalic(true);
    subsectionFormat.setFontItalic(true);
    questionFormat.setFontItalic(true);
    correctAnswerFormat.setFontItalic(false);
    incorrectAnswerFormat.setFontItalic(false);
    priceFormat.setFontItalic(false);

    keywordFormat.setFontUnderline(false);
    sectionFormat.setFontUnderline(true);
    ticketFormat.setFontUnderline(true);
    subsectionFormat.setFontUnderline(true);
    questionFormat.setFontUnderline(false);
    correctAnswerFormat.setFontUnderline(false);
    incorrectAnswerFormat.setFontUnderline(false);
    priceFormat.setFontUnderline(false);

    load_color();
}


QString MyHighlighter::espace(QString s)
{
    return(s.replace("\\", "\\\\")
               .replace("*", "\\*")
               .replace("$", "\\$")
               .replace("?", "\\?")
           );
}


void MyHighlighter::highlightBlock(const QString &text)
{
    QTextCharFormat myClassFormat;


    myClassFormat.setFontWeight(QFont::Bold);
    myClassFormat.setForeground(Qt::darkMagenta);


    QRegularExpressionMatchIterator i;


    i = sectionExpression.globalMatch(text);

    while (i.hasNext() )
    {
        QRegularExpressionMatch match = i.next();


        setFormat(match.capturedStart(), match.capturedLength(), sectionFormat);
        setFormat(match.capturedStart(1), match.capturedLength(1), keywordFormat);
    }
    i = subsectionExpression.globalMatch(text);

    while (i.hasNext() )
    {
        QRegularExpressionMatch match = i.next();


        setFormat(match.capturedStart(), match.capturedLength(), subsectionFormat);
        setFormat(match.capturedStart(1), match.capturedLength(1), keywordFormat);
    }
    i = ticketExpression.globalMatch(text);

    while (i.hasNext() )
    {
        QRegularExpressionMatch match = i.next();


        setFormat(match.capturedStart(), match.capturedLength(), ticketFormat);
        setFormat(match.capturedStart(1), match.capturedLength(1), keywordFormat);
    }
    i = questionExpression.globalMatch(text);

    while (i.hasNext() )
    {
        QRegularExpressionMatch match = i.next();


        setFormat(match.capturedStart(), match.capturedLength(), questionFormat);
        setFormat(match.capturedStart(2), match.capturedLength(2), priceFormat);
        setFormat(match.capturedStart(1), match.capturedLength(1), keywordFormat);
    }
    i = incorrectAnswerExpression.globalMatch(text);

    while (i.hasNext() )
    {
        QRegularExpressionMatch match = i.next();


        setFormat(match.capturedStart(), match.capturedLength(), incorrectAnswerFormat);
        setFormat(match.capturedStart(2), match.capturedLength(2), priceFormat);
        setFormat(match.capturedStart(1), match.capturedLength(1), keywordFormat);
    }
    i = correctAnswerExpression.globalMatch(text);

    while (i.hasNext() )
    {
        QRegularExpressionMatch match = i.next();


        setFormat(match.capturedStart(), match.capturedLength(), correctAnswerFormat);
        setFormat(match.capturedStart(2), match.capturedLength(2), priceFormat);
        setFormat(match.capturedStart(1), match.capturedLength(1), keywordFormat);
    }
    //    i = keywordExpression.globalMatch(text);
    //    while (i.hasNext())
    //    {
    //        QRegularExpressionMatch match = i.next();
    //        setFormat(match.capturedStart(), match.capturedLength(), keywordFormat);
    //    }

    i = matchExpression.globalMatch(text);

    while (i.hasNext() )
    {
        QRegularExpressionMatch match = i.next();


        setFormat(match.capturedStart(1), match.capturedLength(1), keywordFormat);
    }
} // MyHighlighter::highlightBlock


void load_format(QSettings &settings, QString section, QTextCharFormat &format)
{
    format.setFontItalic(settings.value(section + "/italic", format.fontItalic() ).toBool() );
    format.setFontUnderline(settings.value(section + "/underline", format.fontUnderline() ).toBool() );
    format.setFontWeight(settings.value(section + "/weight", format.fontWeight() ).toInt() );
    format.setForeground(QColor(settings.value(section + "/color", format.foreground().color().name() ).toString() ) );
}


void MyHighlighter::update_re(bool multiline, bool markers)
{
    // TODO: переключение форматов
    if (markers)
    {
        correctAnswerExpression.setPattern("^\\s*(-?\\*|-?\\+)[^\\n]*");
        incorrectAnswerExpression.setPattern("^\\s*(-)[^*][^#@\\?\\*$][^\\n]*");
    } else {
        correctAnswerExpression.setPattern("^\\s*(\\*)(\\d*.?\\d*%%)?[^\\n]*");
        incorrectAnswerExpression.setPattern("^\\s*(-\\d*.?\\d*%%)?[^#@\\?\\*$][^\\n]*");
    }

    this->rehighlight();
}


void MyHighlighter::load_color()
{
    QSettings settings("TSU", "TestConvert");

    load_format(settings, "keywordFormat", keywordFormat);
    load_format(settings, "sectionFormat", sectionFormat);
    load_format(settings, "subsectionFormat", subsectionFormat);
    load_format(settings, "ticketFormat", ticketFormat);
    load_format(settings, "questionFormat", questionFormat);
    load_format(settings, "correctAnswerFormat", correctAnswerFormat);
    load_format(settings, "incorrectAnswerFormat", incorrectAnswerFormat);
    load_format(settings, "priceFormat", priceFormat);
}


void save_format(QSettings &settings, QString section, QTextCharFormat &format)
{
    settings.setValue(section + "/italic", format.fontItalic() );
    settings.setValue(section + "/underline", format.fontUnderline() );
    settings.setValue(section + "/weight", format.fontWeight() );
    settings.setValue(section + "/color", format.foreground().color().name() );
}


void MyHighlighter::save_color()
{
    QSettings settings("TSU", "TestConvert");

    save_format(settings, "keywordFormat", keywordFormat);
    save_format(settings, "sectionFormat", sectionFormat);
    save_format(settings, "subsectionFormat", subsectionFormat);
    save_format(settings, "ticketFormat", ticketFormat);
    save_format(settings, "questionFormat", questionFormat);
    save_format(settings, "correctAnswerFormat", correctAnswerFormat);
    save_format(settings, "incorrectAnswerFormat", incorrectAnswerFormat);
    save_format(settings, "priceFormat", priceFormat);
}
