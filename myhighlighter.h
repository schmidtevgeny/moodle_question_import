#ifndef MYHIGHLIGHTER_H
#define MYHIGHLIGHTER_H
#include <QSyntaxHighlighter>
#include <QRegularExpression>
class MyHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

  public:
    MyHighlighter(QTextDocument * parent = 0);

  protected:
    void highlightBlock(const QString & text) override;

  private:
    QTextCharFormat keywordFormat;
    QTextCharFormat sectionFormat;
    QTextCharFormat ticketFormat;
    QTextCharFormat subsectionFormat;
    QTextCharFormat questionFormat;
    QTextCharFormat correctAnswerFormat;
    QTextCharFormat incorrectAnswerFormat;
    QRegularExpression sectionExpression;
    QRegularExpression ticketExpression;
    QRegularExpression incorrectAnswerExpression;
    QRegularExpression keywordExpression;
    QRegularExpression correctAnswerExpression;
    QRegularExpression questionExpression;
    QRegularExpression subsectionExpression;
    QRegularExpression matchExpression;
};

#endif    // MYHIGHLIGHTER_H
