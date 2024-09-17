#include "passwordrecoverydialog.h"
#include "ui_passwordrecoverydialog.h"

PasswordRecoveryDialog::PasswordRecoveryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordRecoveryDialog)
{
    ui->setupUi(this);


    //移除窗口标志
    // 在构造函数中添加以下代码
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);

    //设置提示语
    ui->passwordLineEdit->setPlaceholderText("密码(8-16个字符，仅大小写字母、数字)");


    //设置随机验证码
    ui->showCaptchaLabel->setPixmap(captchaGenerator::refreshCaptcha());

    connect(ui->phoneNumberLineEdit,&QLineEdit::textChanged,this,&PasswordRecoveryDialog::validatePhoneNumber);
    connect(ui->captchaLineEdit,&QLineEdit::textChanged,this,&PasswordRecoveryDialog::onCaptchaChanged);
    connect(ui->passwordLineEdit,&QLineEdit::textChanged,this,&PasswordRecoveryDialog::passwordLineEdit);
    connect(ui->confirmPasswordLineEdit,&QLineEdit::textChanged,this,&PasswordRecoveryDialog::onConfirmPasswordChanged);
    connect(ClientSocket::instance(),&ClientSocket::ForgetPasswordSuccessfully,this,&PasswordRecoveryDialog::on_cancelButton_clicked);
    connect(ClientSocket::instance(),&ClientSocket::PhoneNumberNotExist,this,&PasswordRecoveryDialog::NotValidatePhoneNumber);
}

PasswordRecoveryDialog::~PasswordRecoveryDialog()
{
    delete ui;
}

void PasswordRecoveryDialog::validatePhoneNumber(const QString &text)
{
    QString errorMessage;

    QRegExp regExp("^1[3-9]\\d{9}$");
    bool ret =regExp.exactMatch(text);

    if(!ret)
    {
        errorMessage = "手机号码无效";
    }
    else
    {
        //发送手机号码给服务端  判断手机号是否有效
        
        //构建JSOn对象
        QJsonObject phoneNumberInfo;
        phoneNumberInfo["phoneNumber"]=text;
        
        //封包
        Packet phoneNumberPacket =Protocol::createPacket(PhoneNumberIsExistRequest,phoneNumberInfo);
        
        //序列化
        QByteArray dataToSend = Protocol::serializePacket(phoneNumberPacket);
        
        ClientSocket::instance()->senData(dataToSend);
        
    }

    if(errorMessage.isEmpty())
    {
        ui->phoneNumberErrorlabel->setText("");
        inputDateFlag.push_back(true);
    }
    else
    {
        ui->phoneNumberErrorlabel->setStyleSheet("color:red");
        ui->phoneNumberErrorlabel->setText(errorMessage);
    }
}

void PasswordRecoveryDialog::onCaptchaChanged(const QString &text)
{
    QString errorMessage;

    QString captcha =captchaGenerator::getCaptchaText().toLower();

    if(text.toLower()!=captcha)
    {
        errorMessage ="验证码输入有误";
    }

    if(errorMessage.isEmpty())
    {
        ui->captchaErrorLabel->setText("");
        inputDateFlag.push_back(true);
    }
    else
    {
        ui->captchaErrorLabel->setStyleSheet("color:red;");
        ui->captchaErrorLabel->setText(errorMessage);
    }
}

void PasswordRecoveryDialog::passwordLineEdit(const QString &text)
{
    QString errorMessage;
     QRegExp regex("^(?=.*[A-Z])(?=.*[a-z])(?=.*\\d)[A-Za-z\\d]{8,16}$");
     bool ret =regex.exactMatch(text);

     if(!ret)
     {
         errorMessage = "密码必须是8到16个字符，包含大小写字母和数字";
     }

     if(errorMessage.isEmpty())
     {
         ui->passwordErrorLabel->setText("");
         inputDateFlag.push_back(true);
     }
     else
     {
         ui->passwordErrorLabel->setStyleSheet("color:red;");
         ui->passwordErrorLabel->setText(errorMessage);
     }


}

void PasswordRecoveryDialog::onConfirmPasswordChanged(const QString &text)
{
    QString errorMessage;
    QString password = ui->passwordLineEdit->text();

    if(text != password)
    {
        ui->confirmPasswordErrorLabel->setText("");
        inputDateFlag.push_back(true);
    }
    else
    {
        ui->confirmPasswordErrorLabel->setStyleSheet("color:red");
        ui->confirmPasswordErrorLabel->setText(errorMessage);
    }
}



void PasswordRecoveryDialog::on_reflushButton_clicked()
{
    ui->showCaptchaLabel->setPixmap(captchaGenerator::refreshCaptcha());
    QString captcha =captchaGenerator::getCaptchaText();
    qDebug()<<captcha;
}


void PasswordRecoveryDialog::on_okButton_clicked()
{
    if(std::find(inputDateFlag.begin(),inputDateFlag.end(),false)!=inputDateFlag.end())
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("警告");
        msgBox.setText("输入信息不完整");
        msgBox.setIcon(QMessageBox::Warning);

        QTimer::singleShot(2000,&msgBox,&QMessageBox::close);

        msgBox.exec();

        return;
    }

    //获取输入框的内容
    QString phoneNumber = ui->phoneNumberLineEdit->text();
    QString password =ui->passwordLineEdit->text();

    //构造JSON对象
    QJsonObject forgetPasswordInfo;
    forgetPasswordInfo["phoneNumber"] = phoneNumber;
    forgetPasswordInfo["password"] =password;

    //封包
    Packet packet =Protocol::createPacket(ForGetPasswordRequest,forgetPasswordInfo);

    //序列化
    QByteArray dataToSend = Protocol::serializePacket(packet);

    //发送给服务端
    ClientSocket::instance()->senData(dataToSend);

}


void PasswordRecoveryDialog::on_cancelButton_clicked()
{
    this->hide();
}

void PasswordRecoveryDialog::NotValidatePhoneNumber(const QString &text)
{
    ui->phoneNumberErrorlabel->setStyleSheet("color:red;");
    ui->phoneNumberErrorlabel->setText(text);
    inputDateFlag.push_back(false);
}

