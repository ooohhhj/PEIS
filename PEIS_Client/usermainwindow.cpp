#include "usermainwindow.h"
#include "ui_usermainwindow.h"


UserMainWindow::UserMainWindow(QWidget *parent,const QString &username) :
    QMainWindow(parent),m_username(username),
    ui(new Ui::UserMainWindow)
{
    ui->setupUi(this);

    ui->usernameLabel->setText(this->m_username);

    //添加预约体检界面到StackWidget中
    scheduleCheckUp =std::make_unique<ScheduleCheckup>(this);
    hepDetails =std::make_unique<HEPDetails>(this);
    checkupRecord =std::make_unique<CheckupRecord>(this);
    checkupreport =std::make_unique<CheckupReport>(this);
    healthexaminationreport =std::make_unique<HealthExaminationReport>(this);

    scheduleCheckUp.get()->setUsername(this->m_username);
    hepDetails.get()->setUsername(this->m_username);
    checkupreport.get()->setUsername(this->m_username);
    checkupRecord.get()->setUsername(this->m_username);

    setDefaultWidget();

    ui->stackedWidget->addWidget(scheduleCheckUp.get());
    ui->stackedWidget->addWidget(hepDetails.get());
    ui->stackedWidget->addWidget(checkupRecord.get());
    ui->stackedWidget->addWidget(checkupreport.get());
    ui->stackedWidget->addWidget(healthexaminationreport.get());


    //   设置只允许最大化，不允许最小化
    this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |Qt::WindowCloseButtonHint);

    //显示窗口为最大化状态
    this->showMaximized();


    connect(scheduleCheckUp.get(),&ScheduleCheckup::cardClicked,this,&UserMainWindow::OnCardClicked);

    connect(scheduleCheckUp.get(),&ScheduleCheckup::exitButtonClicked,this,&UserMainWindow::setDefaultWidget);
    connect(scheduleCheckUp.get(),&ScheduleCheckup::exitButtonClicked,this,&UserMainWindow::buttonStyleSheet);

    connect(hepDetails.get(),&HEPDetails::exitButtonClicked,this,&UserMainWindow::on_scheduleCheckupButton_clicked);

    connect(checkupRecord.get(),&CheckupRecord::exitButtonClicked,this,&UserMainWindow::setDefaultWidget);
    connect(checkupRecord.get(),&CheckupRecord::exitButtonClicked,this,&UserMainWindow::buttonStyleSheet);


    connect(checkupreport.get(),&CheckupReport::exitButtonClicked,this,&UserMainWindow::setDefaultWidget);
    connect(checkupreport.get(),&CheckupReport::exitButtonClicked,this,&UserMainWindow::buttonStyleSheet);


    connect(healthexaminationreport.get(),&HealthExaminationReport::exitButtonClicked,this,&UserMainWindow::on_healthCheckRecordButton_clicked);

    connect(ClientSocket::instance(),&ClientSocket::OnUserCheckupGenerateNotice,this,&UserMainWindow::updatenoticeButton);

    connect(checkupreport.get(),&CheckupReport::LookCheckupreport,this,&UserMainWindow::LookCheckupreport);
    connect(checkupRecord.get(),&CheckupRecord::LookCheckupreport,this,&UserMainWindow::LookCheckupreport);


}

UserMainWindow::~UserMainWindow()
{
    delete ui;
}

void UserMainWindow::setUsername(const QString &username)
{
    this->m_username =username;
}

void UserMainWindow::on_scheduleCheckupButton_clicked()
{

    ui->stackedWidget->setCurrentWidget(scheduleCheckUp.get());


    ui->scheduleCheckupButton->setStyleSheet(
                "QPushButton {"
                "    background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #A4D0E1, stop: 1 #B0E0E6);"
                "    border: 1px solid #8f8f91;"
                "    border-radius: 5px;"
                "    padding: 5px;"
                "}"
                "QPushButton:pressed, QPushButton:checked {"
                "    background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #88B0C1, stop: 1 #90C0C6);"
                "}"
                );

    // 确保按钮可切换
    ui->scheduleCheckupButton->setCheckable(true);

    // 设置按钮为选中状态
    ui->scheduleCheckupButton->setChecked(true);


    //发送信息给服务端  获取套餐内容

    QJsonObject Info;

    //获取页数 和页数量
    Info["currentPage"] = scheduleCheckUp.get()->getCurrentPage();
    Info["itemsPerPage"] =scheduleCheckUp.get()->getItemsPerPage();

    //封包
    Packet reserveCheckupPacket =Protocol::createPacket(ReserveCheckupRequest,Info);

    //序列化
    QByteArray dataToSend = Protocol::serializePacket(reserveCheckupPacket);

    ClientSocket::instance()->senData(dataToSend);

}

void UserMainWindow::OnCardClicked(const QString &cardName)
{
    //切换界面
    ui->stackedWidget->setCurrentWidget(hepDetails.get());

    hepDetails->setCardName(cardName);

    //发送请求
    QJsonObject Info;

    Info["packageName"] =cardName;

    //封包
    Packet packet = Protocol::createPacket(PackageInformationRequest,Info);

    //序列化
    QByteArray dataToSend = Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(dataToSend);

}

void UserMainWindow::setDefaultWidget()
{
    // 创建一个新的 QWidget 作为堆栈窗口
    QWidget *centralWidget = new QWidget(this);

    // 创建一个垂直布局
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // 创建 QLabel 用于显示图像
    QLabel *imageLabel = new QLabel(this);
    QPixmap image(":/pexels-gustavo-fring.jpg"); // 确保在资源文件中包含该图像
    imageLabel->setPixmap(image);
    imageLabel->setScaledContents(true); // 使图像适应标签大小

    // 设置 QLabel 的大小策略
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 允许图像标签扩展

    // 将 QLabel 添加到布局中
    layout->addWidget(imageLabel);

    // 设置中心部件的布局
    centralWidget->setLayout(layout);

    // 将 QWidget 添加到 stackedWidget
    ui->stackedWidget->addWidget(centralWidget);

    // 设置当前显示的部件为新创建的部件
    ui->stackedWidget->setCurrentWidget(centralWidget);

    // 确保 stackedWidget 也能最大化
    ui->stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

}

void UserMainWindow::buttonStyleSheet()
{
    QString buttonStyleSheet =
            "QPushButton {"
            "    background-color: transparent;"
            "    border: 2px solid black;"
            "    border-radius: 15px;"
            "    padding: 10px;"
            "}"
            "QPushButton:hover {"
            "    background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #A4D0E1, stop: 1 #B0E0E6);"
            "}"
            "QPushButton:pressed {"
            "    background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #A4D0E1, stop: 1 #B0E0E6);"
            "}";

    ui->scheduleCheckupButton->setStyleSheet(buttonStyleSheet);

    ui->healthCheckRecordButton->setStyleSheet(buttonStyleSheet);

    ui->checkupReportButton->setStyleSheet(buttonStyleSheet);

    ui->personalInfoButton->setStyleSheet(buttonStyleSheet);

}

void UserMainWindow::on_healthCheckRecordButton_clicked()
{

    ui->stackedWidget->setCurrentWidget(checkupRecord.get());

    ui->healthCheckRecordButton->setStyleSheet(
                "QPushButton {"
                "    background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #A4D0E1, stop: 1 #B0E0E6);"
                "    border: 1px solid #8f8f91;"
                "    border-radius: 5px;"
                "    padding: 5px;"
                "}"
                "QPushButton:pressed, QPushButton:checked {"
                "    background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #88B0C1, stop: 1 #90C0C6);"
                "}"
                );

    // 确保按钮可切换
    ui->healthCheckRecordButton->setCheckable(true);

    // 设置按钮为选中状态
    ui->healthCheckRecordButton->setChecked(true);


    //发送记录申请
    QJsonObject Info;

    Info["username"] =m_username;
    //封包
    Packet reserveCheckupPacket =Protocol::createPacket(QueryHealthExaminationRecordsRequest,Info);

    //序列化
    QByteArray dataToSend = Protocol::serializePacket(reserveCheckupPacket);

    ClientSocket::instance()->senData(dataToSend);

}

void UserMainWindow::on_checkupReportButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(checkupreport.get());

    ui->checkupReportButton->setStyleSheet(
                "QPushButton {"
                "    background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #A4D0E1, stop: 1 #B0E0E6);"
                "    border: 1px solid #8f8f91;"
                "    border-radius: 5px;"
                "    padding: 5px;"
                "}"
                "QPushButton:pressed, QPushButton:checked {"
                "    background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #88B0C1, stop: 1 #90C0C6);"
                "}"
                );

    // 确保按钮可切换
    ui->checkupReportButton->setCheckable(true);

    // 设置按钮为选中状态
    ui->checkupReportButton->setChecked(true);

    //发送记录申请
    QJsonObject Info;

    Info["username"] =m_username;
    //封包
    Packet reserveCheckupPacket =Protocol::createPacket(GetHealthExaminationRePortListRequest,Info);

    //序列化
    QByteArray dataToSend = Protocol::serializePacket(reserveCheckupPacket);

    ClientSocket::instance()->senData(dataToSend);
}

void UserMainWindow::on_noticeButton_clicked()
{
    QIcon icon(":/notice-No.png"); // 替换成你想要的图标路径
    ui->noticeButton->setIcon(icon);
}

void UserMainWindow::updatenoticeButton()
{
    QIcon icon(":/notice-Yes.png"); // 替换成你想要的图标路径
    ui->noticeButton->setIcon(icon);
    qDebug()<<"来提醒";
}

void UserMainWindow::LookCheckupreport()
{
    ui->stackedWidget->setCurrentWidget(healthexaminationreport.get());

    ui->checkupReportButton->setStyleSheet(
                "QPushButton {"
                "    background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #A4D0E1, stop: 1 #B0E0E6);"
                "    border: 1px solid #8f8f91;"
                "    border-radius: 5px;"
                "    padding: 5px;"
                "}"
                "QPushButton:pressed, QPushButton:checked {"
                "    background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #88B0C1, stop: 1 #90C0C6);"
                "}"
                );

    // 确保按钮可切换
    ui->checkupReportButton->setCheckable(true);

    // 设置按钮为选中状态
    ui->checkupReportButton->setChecked(true);

}




