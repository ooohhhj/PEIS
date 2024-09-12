#include "registerdialog.h"
#include "ui_registerdialog.h"
#include <QDate>

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);

    //获取当前日期
    QDate currentDate = QDate::currentDate();

    //设置出生日期的最大日期为当前日期
    ui->birthDateEdit->setMaximumDate(currentDate);


}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}
