#include "healthexaminationreport.h"
#include "ui_healthexaminationreport.h"

HealthExaminationReport::HealthExaminationReport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HealthExaminationReport)
{
    ui->setupUi(this);
}

HealthExaminationReport::~HealthExaminationReport()
{
    delete ui;
}
