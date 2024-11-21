#include "dialogconfig.h"
#include "ui_dialogconfig.h"
#include <QSettings>

DialogConfig::DialogConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConfig)
{
    ui->setupUi(this);
    QSettings*  iniFile= new QSettings("TSU", "TestConvert");

    ui->hasAnswerCorrect->setChecked(    iniFile->value("markers/hasAnswerCorrect", false).toBool());
    ui->hasAnswerIncorrect->setChecked(    iniFile->value("markers/hasAnswerIncorrect", false).toBool());
    ui->hasNoShuffle->setChecked(    iniFile->value("markers/hasNoShuffle", false).toBool());
    ui->hasQuestionFinish->setChecked(    iniFile->value("markers/hasQuestionFinish", false).toBool());
    ui->hasQuestionStart->setChecked(    iniFile->value("markers/hasQuestionStart", false).toBool());

    ui->eAnswerCorrect->setText(iniFile->value("markers/eAnswerCorrect", "+").toString());
    ui->eAnswerIncorrect->setText(iniFile->value("markers/eAnswerIncorrect", "-").toString());
    ui->eCloze->setText(iniFile->value("markers/eCloze", "$").toString());
    ui->eCorrect->setText(iniFile->value("markers/eCorrect", "*").toString());
    ui->eMatch->setText(iniFile->value("markers/eMatch", "->").toString());
    ui->eNoShuffle->setText(iniFile->value("markers/eNoShuffle", "!").toString());
    ui->eQuestion->setText(iniFile->value("markers/eQuestion", "?").toString());
    ui->eQuestionFinish->setText(iniFile->value("markers/eQuestionFinish", ">?").toString());
    ui->eQuestionStart->setText(iniFile->value("markers/eQuestionStart", "?<").toString());
    ui->eSection->setText(iniFile->value("markers/eSection", "#").toString());
    ui->eSubsection->setText(iniFile->value("markers/eSubsection", "@").toString());

    delete iniFile;
}

DialogConfig::~DialogConfig()
{
    delete ui;
}

void DialogConfig::on_DialogConfig_accepted()
{
    QSettings*  iniFile= new QSettings("TSU", "TestConvert");

    iniFile->setValue("markers/hasAnswerCorrect", ui->hasAnswerCorrect->isChecked());
    iniFile->setValue("markers/hasAnswerIncorrect", ui->hasAnswerIncorrect->isChecked());
    iniFile->setValue("markers/hasNoShuffle", ui->hasNoShuffle->isChecked());
    iniFile->setValue("markers/hasQuestionFinish", ui->hasQuestionFinish->isChecked());
    iniFile->setValue("markers/hasQuestionStart", ui->hasQuestionStart->isChecked());

    iniFile->setValue("markers/eAnswerCorrect", ui->eAnswerCorrect->text());
    iniFile->setValue("markers/eAnswerIncorrect", ui->eAnswerIncorrect->text());
    iniFile->setValue("markers/eCloze", ui->eCloze->text());
    iniFile->setValue("markers/eCorrect", ui->eCorrect->text());
    iniFile->setValue("markers/eMatch", ui->eMatch->text());
    iniFile->setValue("markers/eNoShuffle", ui->eNoShuffle->text());
    iniFile->setValue("markers/eQuestion", ui->eQuestion->text());
    iniFile->setValue("markers/eQuestionFinish", ui->eQuestionFinish->text());
    iniFile->setValue("markers/eQuestionStart", ui->eQuestionStart->text());
    iniFile->setValue("markers/eSection", ui->eSection->text());
    iniFile->setValue("markers/eSubsection", ui->eSubsection->text());

    delete iniFile;

}

