#include "schedulecheckup.h"
#include "ui_schedulecheckup.h"


ScheduleCheckup::ScheduleCheckup(QWidget *parent) :
    QWidget(parent),currentPage(1),itemsPerPage(3),
    ui(new Ui::ScheduleCheckup)
{
    ui->setupUi(this);

    searchFlag =false;

    this->setStyleSheet("border: 2px solid #B0C4DE;"  // 边框为银灰色
                        "border-radius: 13px;"        // 圆角，内角半径为15px
                        "box-shadow: inset 4px 4px 10px rgba(0, 0, 0, 0.1);");


    connect(ClientSocket::instance(),&ClientSocket::ReserveCheckup,this,&ScheduleCheckup::reserveCheckup);

    connect(ClientSocket::instance(),&ClientSocket::onGetAppointmentInfo,this,&ScheduleCheckup::onGetAppointmentInfo);

}

ScheduleCheckup::~ScheduleCheckup()
{
    delete ui;
}

void ScheduleCheckup::requestPackagesForPage(int page)
{
    QJsonObject info;
    info["currentPage"] =page;// 当前页码
    info["itemsPerPage"] = itemsPerPage;// 每页显示数量

    //封包分页查询请求
    Packet packageQueryPacket ;
    if(!searchFlag)
    {
        packageQueryPacket= Protocol::createPacket(UpdateCheckupPackageRequest,info);
    }
    else
    {
        packageQueryPacket= Protocol::createPacket(UpdateSearchPackageRequest,info);
    }

    QByteArray dataToSend =Protocol::serializePacket(packageQueryPacket);

    ClientSocket::instance()->senData(dataToSend);

}

int ScheduleCheckup::getCurrentPage()
{
    return this->currentPage;
}

int ScheduleCheckup::getItemsPerPage()
{
    return this->itemsPerPage;
}

void ScheduleCheckup::reserveCheckup(const QJsonArray &packagesArray , const int &totalPage)
{
    if(totalPage == 0)
    {
        ClientSocket::instance()->showMessageBox(":/warning.png","预约套餐","套餐不存在");
        ui->searchLineEdit->setText("");
        return;
    }
    //设置图标和文本
    // 更新卡片1内容
    if (packagesArray.size() > 0) {
        QJsonObject package1 = packagesArray[0].toObject();
        ui->cardNameButton->setText(package1["package_name"].toString());
        ui->TALabel->setText(package1["target_population"].toString());
        ui->HCELabel->setText(package1["provider"].toString());
        ui->PriceLabel->setText(QString::number(package1["price"].toDouble()));
        ui->PriceLabel->setScaledContents(true);
        ui->cardwidget->setVisible(true);

    }
    else{
        ui->cardwidget->setVisible(false);
    }

    // 更新卡片2内容
    if (packagesArray.size() > 1) {
        QJsonObject package2 = packagesArray[1].toObject();
        ui->cardNameButton_2->setText(package2["package_name"].toString());
        ui->TALabel_2->setText(package2["target_population"].toString());
        ui->HCELabel_2->setText(package2["provider"].toString());
        ui->PriceLabel_2->setText(QString::number(package2["price"].toDouble()));
        ui->PriceLabel_2->setScaledContents(true);
        ui->cardwidget_2->setVisible(true);
    }
    else
    {
        ui->cardwidget_2->setVisible(false);
    }

    // 更新卡片3内容
    if (packagesArray.size() > 2) {
        QJsonObject package3 = packagesArray[2].toObject();
        ui->cardNameButton_3->setText(package3["package_name"].toString());
        ui->TALabel_3->setText(package3["target_population"].toString());
        ui->HCELabel_3->setText(package3["provider"].toString());
        ui->PriceLabel_3->setText(QString::number(package3["price"].toDouble()));
        ui->PriceLabel_3->setScaledContents(true);
        ui->cardwidget_3->setVisible(true);
    }
    else
    {
        ui->cardwidget_3->setVisible(false);
    }

    ui->totalPageLabel->setText(QString::number(totalPage));
    ui->curPageLabel ->setText(QString::number(this->currentPage));


    //控制分页按钮的显示和隐藏
    ui->prevBtn->setVisible(this->currentPage>1);
    ui->nextBtn->setVisible(this->currentPage<totalPage);

}

void ScheduleCheckup::setUsername(const QString &username)
{
    this->m_username =username;
}

void ScheduleCheckup::on_nextBtn_clicked()
{

    currentPage++;

    requestPackagesForPage(currentPage);

}


void ScheduleCheckup::on_prevBtn_clicked()
{
    if(currentPage>1)
    {
        currentPage--;
        requestPackagesForPage(currentPage);
    }
}


void ScheduleCheckup::on_cardNameButton_clicked()
{
    //获取套餐名称
    QString cardName= ui->cardNameButton->text();
    //发送套餐名称
    emit cardClicked(cardName);
}


void ScheduleCheckup::on_cardNameButton_2_clicked()
{
    //获取套餐名称
    QString cardName= ui->cardNameButton_2->text();
    //发送套餐名称
    emit cardClicked(cardName);
}


void ScheduleCheckup::on_cardNameButton_3_clicked()
{
    //获取套餐名称
    QString cardName= ui->cardNameButton_3->text();
    //发送套餐名称
    emit cardClicked(cardName);
}


void ScheduleCheckup::on_pushButton_clicked()
{
    //获取搜索框
    QString searchLineEdit =ui->searchLineEdit->text();

    QJsonObject obj;

    obj["searchLineEdit"]=searchLineEdit;

    //发送消息
    Packet packet =Protocol::createPacket(SearchPackageRequest,obj);

    QByteArray dataTosend = Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(dataTosend);

    searchFlag =true;

}


void ScheduleCheckup::on_returnExitButton_clicked()
{

    emit exitButtonClicked();

}


void ScheduleCheckup::on_reflushButton_clicked()
{
    searchFlag =false;

    ui->searchLineEdit->setText("");

    QJsonObject Info;

    //获取页数 和页数量
    Info["currentPage"] = getCurrentPage();
    Info["itemsPerPage"] =getItemsPerPage();

    qDebug()<<"getItemsPerPage="<<getCurrentPage();
    qDebug()<<"getItemsPerPage="<<getItemsPerPage();

    //封包
    Packet reserveCheckupPacket =Protocol::createPacket(ReserveCheckupRequest,Info);

    //序列化
    QByteArray dataToSend = Protocol::serializePacket(reserveCheckupPacket);

    ClientSocket::instance()->senData(dataToSend);

}


void ScheduleCheckup::on_AppointmentButton_clicked()
{
    //发送预约信息请求

    QJsonObject obj;
    obj["username"] =this->m_username;

    Packet packet =Protocol::createPacket(AppointmentInfoRequest,obj);

    QByteArray array = Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(array);

}

void ScheduleCheckup::onGetAppointmentInfo(const QString &username, const QString &packageName, const QString &appointmentDate, const QString &status)
{

    // 创建对话框
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("预约信息");
    dialog->setWindowFlags(dialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);  // 移除帮助按钮

    // 设置对话框的大小
    dialog->setFixedSize(500, 400);

    // 设置对话框图标
    QIcon dialogIcon(":/appointment.png");  // 替换为你的图标路径
    dialog->setWindowIcon(dialogIcon);

    // 创建控件
    QLabel *nameLabel = new QLabel("姓名: " + username, dialog);
    QLabel *packageLabel = new QLabel("套餐: " + packageName, dialog);
    QLabel *dateLabel = new QLabel("预约时间: " + appointmentDate, dialog);
    QLabel *statusLabel = new QLabel("预约状态: " + status, dialog);

    QPushButton *cancelButton = new QPushButton("取消", dialog);

    QString labelStyle = "background-color: transparent; border: none; font-size: 12pt;";  // 增加字体大小

    nameLabel->setStyleSheet(labelStyle);
    packageLabel->setStyleSheet(labelStyle);
    dateLabel->setStyleSheet(labelStyle);
    statusLabel->setStyleSheet(labelStyle);

    cancelButton->setStyleSheet("font-size: 12pt;");


    // 设置布局
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(nameLabel);
    layout->addWidget(packageLabel);
    layout->addWidget(dateLabel);
    layout->addWidget(statusLabel);
    layout->addWidget(cancelButton);

    dialog->setLayout(layout);

    // 连接信号与槽
    connect(cancelButton, &QPushButton::clicked, this,[this, dialog,username, packageName, appointmentDate]() {
        dialog->close();
        OncancelAppointmentButton(username, packageName, appointmentDate);
    });
    // 以模态方式显示对话框
    dialog->exec();

    // 释放对话框内存
    delete dialog;

}


void ScheduleCheckup::OncancelAppointmentButton(const QString &username, const QString &packageName, const QString &appointmentDate)
{
    qDebug()<<"取消按钮被点击";

    QJsonObject obj;
    obj["username"] =username;
    obj["packageName"]=packageName;
    obj["appointmentDate"] =appointmentDate;

    Packet packet =Protocol::createPacket(CancelAppointmentRequest,obj);

    QByteArray array  = Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(array);




}

