#include "registerdialog.h"
#include "ui_registerdialog.h"



RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);

    //移除窗口标志
    // 在构造函数中添加以下代码
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);


    //设置提示语
    ui->nameLineEdit->setPlaceholderText("请输入用户名(2-10位，仅汉字、字母、数字)");
    ui->passwordLineEdit->setPlaceholderText("密码(8-16个字符，仅大小写字母、数字)");

    //设置随机验证码
    ui->showCaptchaLabel->setPixmap(captchaGenerator::refreshCaptcha());

    //设置出生日期可选择范围
    setBirthDate();

    //
    connect(ui->nameLineEdit,&QLineEdit::textChanged,this,&RegisterDialog::onUsernameChanged);
    connect(ui->passwordLineEdit,&QLineEdit::textChanged,this,&RegisterDialog::onPasswordChanged);
    connect(ui->confirmPasswordLineEdit,&QLineEdit::textChanged,this,&RegisterDialog::onConfirmPasswordChanged);
    connect(ui->captchaLineEdit,&QLineEdit::textChanged,this,&RegisterDialog::onCaptchaChanged);
    connect(ui->idCardLineEdit,&QLineEdit::textChanged,this,&RegisterDialog::validateIdNumber);
    connect(ui->iphoneLineEdit,&QLineEdit::textChanged,this,&RegisterDialog::validatePhoneNumber);
    connect(ui->addressLineEdit,&QLineEdit::textChanged,this,&RegisterDialog::onAddressChanged);

    connect(ClientSocket::instance(),&ClientSocket::usernameIsExist,this,&RegisterDialog::registerUsernameExist);

    connect(ClientSocket::instance(),&ClientSocket::RegisterSuccessfully,this,&RegisterDialog::on_cancelButton_clicked);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::setBirthDate()
{

    //获取当前的年月日
    int currentYear=QDate::currentDate().year();
    int currentMonth =QDate::currentDate().month();
    int currentDay = QDate::currentDate().day();

    //初始化年月日范围为当前年月份范围
    //设置年份范围
    for(int i=1900;i<=currentYear;++i)
    {
        ui->yearComboBox->addItem(QString::number(i));
    }

    //设置月份
    for(int i=1;i<=currentMonth;i++)
    {
        ui->monthComboBox->addItem(QString::number(i));
    }

    //设置天数
    for(int i=1;i<=currentDay;i++)
    {
        ui->dayComboBox->addItem(QString::number(i));
    }

    //设置当前默认日期为今天
    ui->yearComboBox->setCurrentText(QString::number(currentYear));
    ui->monthComboBox->setCurrentText(QString::number(currentMonth));
    ui->dayComboBox->setCurrentText(QString::number(currentDay));


    //监听年份或月份的变化 并动态更新月份选项或日期选项
    connect(ui->yearComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onYearChanged(int)));
    connect(ui->monthComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onMonthChanged(int)));

}

void RegisterDialog::onYearChanged(int)
{
    //获取选择年份
    int selectedYear= ui->yearComboBox->currentText().toInt();

    //获取当前的年份与月数
    int currentYear = QDate::currentDate().year();
    int currentMonth = QDate::currentDate().month();

    ui->monthComboBox->clear();
    if(selectedYear == currentYear) //选择的年份与当前年份相同
    {
        for(int i=1;i<=currentMonth;i++)
        {
            ui->monthComboBox->addItem(QString::number(i));
        }
        return;
    }
    else
    {
        for(int i=1;i<=12;i++)
        {
            ui->monthComboBox->addItem(QString::number(i));
        }
    }
}

void RegisterDialog::onMonthChanged(int)
{
    //传递年月组件  更新日数
    QComboBox * yearComboBox = findChild<QComboBox*>("yearComboBox");
    QComboBox * monthComboBox  =findChild<QComboBox*>("monthComboBox");
    updateDaysComboBox(yearComboBox,monthComboBox);
}

void RegisterDialog::updateDaysComboBox(QComboBox *yearComboBox, QComboBox *monthComboBox)
{
    int year = yearComboBox->currentText().toInt();
    int month = monthComboBox->currentText().toInt();

    //获取天数
    int daysInMonth = QDate(year,month,1).daysInMonth();

    //清楚旧的日期并重新填充新的日期选项
    ui->dayComboBox->clear();

    for(int i=1;i<=daysInMonth;i++)
    {
        ui->dayComboBox->addItem(QString::number(i));
    }
}


void RegisterDialog::on_refreshCaptchButton_clicked()
{
    ui->showCaptchaLabel->setPixmap(captchaGenerator::refreshCaptcha());
    QString captcha =captchaGenerator::getCaptchaText();
    qDebug()<<captcha;
}

void RegisterDialog::on_okButton_clicked()
{

    //判断输入数据是否有空
    if(std::find(inputDateFlag.begin(),inputDateFlag.end(),false)!=inputDateFlag.end())
    {
        // 创建QMessageBox
        QMessageBox msgBox;
        msgBox.setWindowTitle("警告");
        msgBox.setText("注册信息不完整!!!");
        msgBox.setIcon(QMessageBox::Warning); // 设置警告图标

        // 使用QTimer设置2秒后关闭消息框
        QTimer::singleShot(2000, &msgBox, &QMessageBox::close); // 2000 毫秒 = 2 秒

        // 显示消息框
        msgBox.exec();
        return;
    }

    //获取输入框的内容
    QString username= ui->nameLineEdit->text();//用户名
    QString sex = ui->sexComboBox->currentText();//性别

    QString birthDate;//出生日期
    QString year = ui->yearComboBox->currentText();//年
    QString month = ui->monthComboBox->currentText();//月
    QString day = ui->dayComboBox->currentText();//日
    birthDate =year+"-"+month+"-"+day;

    QString idNumber = ui->idCardLineEdit->text();//身份证号码
    QString address = ui->addressLineEdit->text();//地址
    QString iphoneNumber = ui->iphoneLineEdit->text();//手机号
    QString password = ui->passwordLineEdit->text();//密码


    //构造JSON对象
    QJsonObject registerInfo;
    registerInfo["username"] =username;
    registerInfo["sex"] = sex;
    registerInfo["birthDate"]=birthDate;
    registerInfo["idNumber"] =idNumber;
    registerInfo["address"]=address;
    registerInfo["iphoneNumber"]=iphoneNumber;
    registerInfo["password"]=password;

    //封包
    Packet registerPacket =Protocol::createPacket(RegisterRequest,registerInfo);

    //序列化
    QByteArray dataToSend =Protocol::serializePacket(registerPacket);

    //发送数据到服务端
    ClientSocket::instance()->senData(dataToSend);
}

void RegisterDialog::onUsernameChanged(const QString &text)
{
    QString errorMessage;


    if(text.isEmpty())
    {
        errorMessage="用户名不能为空";
    }

    else if(text.length()<2 || text.length()>10)
    {
        errorMessage="用户名长度为2-10位字符";
    }
    else if(!text.contains(QRegExp("^[\\u4e00-\\u9fa5\\w]{2,20}$")))
    {
        errorMessage="用户名包含非法字符";
    }
    else
    {
        //判断用户名是否唯一

        //构建JSOn对象
        QJsonObject usernameInfo;
        usernameInfo["username"] = text;

        //封包
        Packet usernamePacket = Protocol::createPacket(UsernameIsExistRequest,usernameInfo);

        //序列化
        QByteArray dataToSend = Protocol::serializePacket(usernamePacket);

        //发送到服务端
        ClientSocket::instance()->senData(dataToSend);
    }


    if(!errorMessage.isEmpty())
    {
        ui->usernameErrorLabel->setText("");//清除错误消息
        inputDateFlag.push_back(true);
    }
    else
    {
        ui->usernameErrorLabel->setStyleSheet("color:red;");
        ui->usernameErrorLabel->setText(errorMessage);//显示错误信息
    }
}

void RegisterDialog::validateIdNumber(const QString &text)
{
    QString errorMessage;

    QRegExp regExp("^\\d{17}[\\dX]$");
    bool ret =regExp.exactMatch(text);
    if(!ret)
    {
        errorMessage ="身份证号码无效";
    }

    if(errorMessage.isEmpty())
    {
        ui->idNumberErrorLabel->setText("");
        inputDateFlag.push_back(true);
    }
    else
    {
        ui->idNumberErrorLabel->setStyleSheet("color:red;");
        ui->idNumberErrorLabel->setText(errorMessage);
    }
}

void RegisterDialog::validatePhoneNumber(const QString &text)
{
    QString errorMessage;

    QRegExp regExp("^1[3-9]\\d{9}$");
    bool ret =regExp.exactMatch(text);

    if(!ret)
    {
        errorMessage ="手机号码无效";
    }

    if(errorMessage.isEmpty())
    {
        ui->iphoneErrorLabel->setText("");
        inputDateFlag.push_back(true);
    }
    else
    {
        ui->iphoneErrorLabel->setStyleSheet("color:red;");
        ui->iphoneErrorLabel->setText(errorMessage);
    }
}

void RegisterDialog::onPasswordChanged(const QString &text)
{
    QString errorMessage;

    QRegExp regex("^(?=.*[A-Z])(?=.*[a-z])(?=.*\\d)[A-Za-z\\d]{8,16}$");
    bool ret =regex.exactMatch(text);

    if(!ret)
    {
        errorMessage="密码必须是8到16个字符，包含大小写字母和数字";
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

void RegisterDialog::onConfirmPasswordChanged(const QString &text)
{
    QString errorMessage;

    QString password =ui->passwordLineEdit->text();

    if(text != password)
    {
        errorMessage="两次密码不匹配";
    }

    if(errorMessage.isEmpty())
    {
        ui->confirmPasswordErrorLabel->setText("");
        inputDateFlag.push_back(true);
    }
    else
    {
        ui->confirmPasswordErrorLabel->setStyleSheet("color:red;");
        ui->confirmPasswordErrorLabel->setText(errorMessage);
    }

}

void RegisterDialog::onCaptchaChanged(const QString &text)
{
    QString errorMessage;

    //获取生成的验证码  并转换为小写  达到不区分大小写效果
    QString captcha =captchaGenerator::getCaptchaText().toLower();

    //用户输入的验证码也转换为小写比较
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

void RegisterDialog::onAddressChanged(const QString &text)
{
    QString errorMessage;

    if(text.isEmpty())
    {
        errorMessage="地址不能为空";
    }
    if(errorMessage.isEmpty())
    {
        ui->addressErrorLabel->setText("");
        inputDateFlag.push_back(true);
    }
    else
    {
        ui->addressErrorLabel->setStyleSheet("color:red");
        ui->addressErrorLabel->setText(errorMessage);
    }

}

void RegisterDialog::registerUsernameExist(const QString &text)
{
    qDebug()<<text;
    ui->usernameErrorLabel->setText(text);
    inputDateFlag.push_back(false);
}

void RegisterDialog::on_cancelButton_clicked()
{
    // 关闭对话框并返回拒绝状态
    this->hide();
}



