#include "textdialog.h"
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>

TextDialog::TextDialog()
{
    lt = new QVBoxLayout(this);
    edit = new QTextEdit();
    lt->addWidget(edit);

    btn = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Save);
    lt->addWidget(btn);
    connect(btn, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(btn, &QDialogButtonBox::rejected, this, &QDialog::reject);

    setWindowTitle(tr("Edit"));
}

QString TextDialog::text()
{
    return edit->toPlainText();
}
void TextDialog::setText(const QString &s)
{
    edit->setPlainText(s);
}
