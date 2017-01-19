#ifndef DEFECT_QUESTION_H
#define DEFECT_QUESTION_H

#include <QDialog>

namespace Ui {
class DefectQuestion;
}

class DefectQuestion : public QDialog
{
    Q_OBJECT

public:
    explicit DefectQuestion(QWidget *parent = 0);
    ~DefectQuestion();
    QString Status();
private:
    Ui::DefectQuestion *ui;
};

#endif // DEFECT_QUESTION_H
