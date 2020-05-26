#include "myhighlighter.h"
#include <QRegularExpression>
// TODO: потом вывести переменные нормально
MyHighlighter::MyHighlighter(QTextDocument * parent)
    : QSyntaxHighlighter(parent),
      sectionExpression("^\\s*#[^\\n]*"),
      ticketExpression("^\\s*$[^\\n]*"),
      correctAnswerExpression("^\\s*\\*[^\\n]*"),
      keywordExpression("^\\s*[#@$\\?\\*]"),
      incorrectAnswerExpression("^\\s*[^#@\\?\\*$][^\\n]*"),
      questionExpression("^\\s*\\?[^\\n]*"),
      subsectionExpression("^\\s*@[^\\n]*"),
      matchExpression("^\\s*\\*[^\\n]*(->)", QRegularExpression::InvertedGreedinessOption) {
    keywordFormat.setFontWeight(QFont::Bold);
    sectionFormat.setFontWeight(QFont::Bold);
    ticketFormat.setFontWeight(QFont::Bold);
    subsectionFormat.setFontWeight(QFont::Bold);
    questionFormat.setFontWeight(QFont::Bold);
    correctAnswerFormat.setFontWeight(QFont::Normal);
    incorrectAnswerFormat.setFontWeight(QFont::Normal);

    keywordFormat.setForeground(Qt::red);
    sectionFormat.setForeground(Qt::black);
    ticketFormat.setForeground(Qt::black);
    subsectionFormat.setForeground(Qt::black);
    questionFormat.setForeground(Qt::black);
    correctAnswerFormat.setForeground(Qt::darkGreen);
    incorrectAnswerFormat.setForeground(Qt::darkMagenta);

    keywordFormat.setFontItalic(false);
    sectionFormat.setFontItalic(true);
    ticketFormat.setFontItalic(true);
    subsectionFormat.setFontItalic(true);
    questionFormat.setFontItalic(true);
    correctAnswerFormat.setFontItalic(false);
    incorrectAnswerFormat.setFontItalic(false);

    keywordFormat.setFontUnderline(false);
    sectionFormat.setFontUnderline(true);
    ticketFormat.setFontUnderline(true);
    subsectionFormat.setFontUnderline(true);
    questionFormat.setFontUnderline(false);
    correctAnswerFormat.setFontUnderline(false);
    incorrectAnswerFormat.setFontUnderline(false);
}
void MyHighlighter::highlightBlock(const QString & text) {
    QTextCharFormat myClassFormat;
    myClassFormat.setFontWeight(QFont::Bold);
    myClassFormat.setForeground(Qt::darkMagenta);

    QRegularExpressionMatchIterator i;
    //    QTextCharFormat sectionFormat;

    i = sectionExpression.globalMatch(text);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), sectionFormat);
    }

    i = subsectionExpression.globalMatch(text);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), subsectionFormat);
    }

    i = ticketExpression.globalMatch(text);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), ticketFormat);
    }

    i = questionExpression.globalMatch(text);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), questionFormat);
    }

    i = incorrectAnswerExpression.globalMatch(text);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), incorrectAnswerFormat);
    }

    i = correctAnswerExpression.globalMatch(text);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), correctAnswerFormat);
    }

    i = keywordExpression.globalMatch(text);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), keywordFormat);
    }

    i = matchExpression.globalMatch(text);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(1), match.capturedLength(1), keywordFormat);
    }
}
