#include "defect_question.h"
#include "ui_defect_question.h"

DefectQuestion::DefectQuestion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DefectQuestion)
{
    ui->setupUi(this);
}

DefectQuestion::~DefectQuestion()
{
    delete ui;
}

QString DefectQuestion::Status()
{
    return ui->comboBox->currentText();
}
