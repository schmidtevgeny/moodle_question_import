#ifndef TEXTDIALOG_H
#define TEXTDIALOG_H

#include <QDialog>
class QVBoxLayout;
class QTextEdit;
class QDialogButtonBox;
class TextDialog : public QDialog
{
    Q_OBJECT
    QVBoxLayout      *lt;
    QTextEdit        *edit;
    QDialogButtonBox *btn;

public:
    TextDialog();
    QString text();
    void setText(const QString &);
};

#endif // TEXTDIALOG_H
