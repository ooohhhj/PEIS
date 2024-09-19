#include "logindialog.h"
#include "ui_logindialog.h"


LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    //初始化roleID
    this->m_userRole = -1;

    this->m_username ="";

    //移除窗口标志
    // 在构造函数中添加以下代码
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);


    //连接服务端
    ClientSocket::instance()->connectToServer("127.0.0.1",9600);

    //固定窗口大小
    this->setFixedSize(500,400);

    //设置用户名与密码字段的提示文本
    ui->phoneNumberOrUsernameLineEdit->setPlaceholderText("手机号码/用户名");
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


    //设置找回密码标签的文本为超链接
    ui->forgotPasswordLabel->setText("<a href ='forgetPassword'>找回密码</a>");

    //设置文本格式为富文本格式 支持HTML
    ui->forgotPasswordLabel->setTextFormat(Qt::RichText);

    //设置交互标志 允许点击
    ui->forgotPasswordLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);

    //启用链接打开 不允许外部链接 内部处理
    ui->forgotPasswordLabel->setOpenExternalLinks(false);

    //连接找回密码标签的点击事件
    connect(ui->forgotPasswordLabel,&QLabel::linkActivated,this,&LoginDialog::onForgetPasswordLabelClicked);


    connect(ClientSocket::instance(),&ClientSocket::Logined,this,&LoginDialog::onLogined);

}

LoginDialog::~LoginDialog()
{
    delete ui;
}

int LoginDialog::getUserRole() const
{
    return this->m_userRole;
}

QString LoginDialog::getUsername() const
{
    return this->m_username;
}

void LoginDialog::onRegisterLabelClicked(const QString &link)
{
    qDebug()<<"注册";
    if(link == "register")
    {
        RegisterDialog registerDialog(this);
        registerDialog.exec() ;
    }

}

void LoginDialog::onForgetPasswordLabelClicked(const QString &link)
{
    qDebug()<<"找回密码";

    if(link == "forgetPassword")
    {
        PasswordRecoveryDialog passwordRecoveryDialog(this);
        passwordRecoveryDialog.exec();
    }
}



void LoginDialog::on_loginButton_clicked()
{
    //获取输入框内容
    QString phoneOrUsername =ui->phoneNumberOrUsernameLineEdit->text();
    QString password =ui->passWordLineEdit->text();

    if(phoneOrUsername.isEmpty() || password.isEmpty())
    {
        ClientSocket::instance()->showMessageBox(":/warning.png","登录","手机号码/用户名 或 密码 为空");
        return;
    }
    else
    {
        int msgType;
        QJsonObject requestJson;

        //判断输入的是手机号还是用户
        QRegExp phoneRegex("^\\d{11}$");
        if(phoneRegex.exactMatch(phoneOrUsername))
        {
            qDebug() << "输入的是手机号";
            msgType =PhoneAndPasswordIsExistRequest;
            requestJson["phoneNumber"] =phoneOrUsername;
        }
        else if(phoneOrUsername.contains(QRegExp("^[\u4e00-\u9fa5a-zA-Z]{2,10}$")))
        {
            qDebug() << "输入的是用户名";
            msgType=UsernameAndPasswordIsExistRequest;
            requestJson["username"] =phoneOrUsername;
        }
        else
        {
            ClientSocket::instance()->showMessageBox(":/warning.png", "登录", "无效的手机号码/用户名格式");
            return; // 输入格式无效，提前返回
        }
        requestJson["password"]=password;

        //封包
        Packet packet = Protocol::createPacket(msgType,requestJson);

        //序列化
        QByteArray dateToSend =Protocol::serializePacket(packet);

        ClientSocket::instance()->senData(dateToSend);

    }
}

void LoginDialog::onLogined(const int &roleId, const QString &username)
{
    this->m_userRole =roleId;
    this->m_username = username;
    qDebug()<<"roleId="<<roleId;
    qDebug()<<"username="<<username;
    this->accept();
}



