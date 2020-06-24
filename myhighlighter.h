#ifndef MYHIGHLIGHTER_H
#define MYHIGHLIGHTER_H
#include <QSyntaxHighlighter>
#include <QRegularExpression>
class MyHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

  public:
    MyHighlighter(QTextDocument * parent = 0);

    QTextCharFormat keywordFormat;
    QTextCharFormat sectionFormat;
    QTextCharFormat subsectionFormat;
    QTextCharFormat ticketFormat;
    QTextCharFormat questionFormat;
    QTextCharFormat correctAnswerFormat;
    QTextCharFormat incorrectAnswerFormat;
    QTextCharFormat priceFormat;

    void load_color();
    void save_color();

  protected:
    void highlightBlock(const QString & text) override;

  private:
    QRegularExpression sectionExpression;
    QRegularExpression ticketExpression;
    QRegularExpression incorrectAnswerExpression;
    QRegularExpression correctAnswerExpression;
    QRegularExpression questionExpression;
    QRegularExpression subsectionExpression;
    QRegularExpression matchExpression;
};

#endif    // MYHIGHLIGHTER_H
