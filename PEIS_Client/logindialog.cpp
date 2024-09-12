#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    //固定窗口大小
    this->setFixedSize(500,400);

    //设置用户名与密码字段的提示文本
    ui->phoneNumberLineEdit->setPlaceholderText("手机号");
    ui->passWordLineEdit->setPlaceholderText("请输入密码");
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
