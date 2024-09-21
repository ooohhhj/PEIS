#include "schedulecheckup.h"
#include "ui_schedulecheckup.h"

ScheduleCheckup::ScheduleCheckup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScheduleCheckup)
{
    ui->setupUi(this);

    this->setStyleSheet("border: 2px solid #B0C4DE;"  // 边框为银灰色
                        "border-radius: 13px;"        // 圆角，内角半径为15px
                        "box-shadow: inset 4px 4px 10px rgba(0, 0, 0, 0.1);");


}

ScheduleCheckup::~ScheduleCheckup()
{
    delete ui;
}
