#include "inputmedicaexaminationdata.h"
#include "ui_inputmedicaexaminationdata.h"

InputMedicaExaminationData::InputMedicaExaminationData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputMedicaExaminationData)
{
    ui->setupUi(this);
}

InputMedicaExaminationData::~InputMedicaExaminationData()
{
    delete ui;
}
