#include "checkupreport.h"
#include "ui_checkupreport.h"

CheckupReport::CheckupReport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CheckupReport)
{
    ui->setupUi(this);
}

CheckupReport::~CheckupReport()
{
    delete ui;
}
