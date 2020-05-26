#ifndef MYHIGHLIGHTER_H
#define MYHIGHLIGHTER_H
#include <QSyntaxHighlighter>

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
    QTextCharFormat answerFormat;
    QTextCharFormat answer2Format;
};

#endif    // MYHIGHLIGHTER_H
