#include "usermainwindow.h"
#include "ui_usermainwindow.h"


UserMainWindow::UserMainWindow(QWidget *parent,const QString &username) :
    QMainWindow(parent),m_username(username),
    ui(new Ui::UserMainWindow)
{
    ui->setupUi(this);

    ui->usernameLabel->setText(this->m_username);
    qDebug()<<"username="<<this->m_username;



    //添加预约体检界面到StackWidget中
    scheduleCheckUp =std::make_unique<ScheduleCheckup>(this);
    hepDetails =std::make_unique<HEPDetails>(this);

    ui->stackedWidget->addWidget(scheduleCheckUp.get());
    ui->stackedWidget->addWidget(hepDetails.get());


    //   设置只允许最大化，不允许最小化
    this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |Qt::WindowCloseButtonHint);

    //显示窗口为最大化状态
    this->showMaximized();

    connect(scheduleCheckUp.get(),&ScheduleCheckup::cardClicked,this,&UserMainWindow::OnCardClicked);

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
