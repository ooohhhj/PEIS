#ifndef INPUTMEDICAEXAMINATIONDATA_H
#define INPUTMEDICAEXAMINATIONDATA_H

#include <QWidget>

namespace Ui {
class InputMedicaExaminationData;
}

class InputMedicaExaminationData : public QWidget
{
    Q_OBJECT

public:
    explicit InputMedicaExaminationData(QWidget *parent = nullptr);
    ~InputMedicaExaminationData();

private:
    Ui::InputMedicaExaminationData *ui;
};

#endif // INPUTMEDICAEXAMINATIONDATA_H
