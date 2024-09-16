#include "logindialog.h"
#include "ui_logindialog.h"
#include "registerdialog.h"
#include "clientsocket.h"
#include<QDebug>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);


    //连接服务端
    ClientSocket::instance()->connectToServer("127.0.0.1",9600);

    //固定窗口大小
    this->setFixedSize(500,400);

    //设置用户名与密码字段的提示文本
    ui->phoneNumberLineEdit->setPlaceholderText("手机号/用户名");
    ui->passWordLineEdit->setPlaceholderText("请输入密码");

    //设置注册标签的文本为超链接格式
    ui->signUpLabel->setText("<a href='register'> 注册账号</a>");

    //设置文本格式为富文本格式 支持HTML
    ui->signUpLabel->setTextFormat(Qt::RichText);
    //设置交互标志，允许点击
    ui->signUpLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    //启用链接打开  不允许外部链接，内部处理
    ui->signUpLabel->setOpenExternalLinks(false);

    //连接注册标签的点击事件
    connect(ui->signUpLabel,&QLabel::linkActivated,this,&LoginDialog::onRegisterLabelClicked);


}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::onRegisterLabelClicked(const QString &link)
{
    qDebug()<<"注册";
    if(link == "register")
    {
        RegisterDialog registerDialog(this);
        registerDialog.exec();
    }

}
