#include "checkuprecord.h"
#include "ui_checkuprecord.h"

CheckupRecord::CheckupRecord(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CheckupRecord)
{
    ui->setupUi(this);

}

CheckupRecord::~CheckupRecord()
{
    delete ui;
}
