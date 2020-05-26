#include "myhighlighter.h"
#include <QRegularExpression>
// TODO: потом вывести переменные нормально
MyHighlighter::MyHighlighter(QTextDocument * parent) : QSyntaxHighlighter(parent) {
    keywordFormat.setFontWeight(QFont::Bold);
    sectionFormat.setFontWeight(QFont::Bold);
    subsectionFormat.setFontWeight(QFont::Bold);
    questionFormat.setFontWeight(QFont::Bold);
    answerFormat.setFontWeight(QFont::Normal);
    answer2Format.setFontWeight(QFont::Normal);

    keywordFormat.setForeground(Qt::blue);
    sectionFormat.setForeground(Qt::black);
    subsectionFormat.setForeground(Qt::black);
    questionFormat.setForeground(Qt::black);
    answerFormat.setForeground(Qt::darkMagenta);
    answer2Format.setForeground(Qt::darkGreen);

    keywordFormat.setFontItalic(false);
    sectionFormat.setFontItalic(true);
    subsectionFormat.setFontItalic(true);
    questionFormat.setFontItalic(true);
    answerFormat.setFontItalic(false);
    answer2Format.setFontItalic(false);

    keywordFormat.setFontUnderline(false);
    sectionFormat.setFontUnderline(true);
    subsectionFormat.setFontUnderline(true);
    questionFormat.setFontUnderline(false);
    answerFormat.setFontUnderline(false);
    answer2Format.setFontUnderline(false);
}
void MyHighlighter::highlightBlock(const QString & text) {
    QTextCharFormat myClassFormat;
    myClassFormat.setFontWeight(QFont::Bold);
    myClassFormat.setForeground(Qt::darkMagenta);

    QRegularExpressionMatchIterator i;
    //    QTextCharFormat sectionFormat;
    QRegularExpression expression1("^\\s*#[^\\n]*");
    i = expression1.globalMatch(text);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), sectionFormat);
    }
    //    QTextCharFormat subsectionFormat;
    QRegularExpression expression2("^\\s*@[^\\n]*");
    i = expression2.globalMatch(text);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), subsectionFormat);
    }
    //    QTextCharFormat questionFormat;
    QRegularExpression expression3("^\\s*\\?[^\\n]*");
    i = expression3.globalMatch(text);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), questionFormat);
    }
    //    QTextCharFormat keywordFormat;
    //    QTextCharFormat answerFormat;
    QRegularExpression expression4("^\\s*\\*[^\\n]*");
    i = expression4.globalMatch(text);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), answerFormat);
    }

    QRegularExpression expression5("^\\s*[^#@\\?\\*][^\\n]*");
    i = expression5.globalMatch(text);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), answer2Format);
    }

    QRegularExpression expression6("^\\s*[#@\\?\\*]");
    i = expression6.globalMatch(text);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), keywordFormat);
    }
}
