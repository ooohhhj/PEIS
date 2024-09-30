#include "adminmainwindow.h"
#include "ui_adminmainwindow.h"

AdminMainWindow::AdminMainWindow(QWidget *parent,const QString &username) :
    QMainWindow(parent),m_username(username),
    ui(new Ui::AdminMainWindow)
{
    ui->setupUi(this);

    ui->usernameLabel->setText(this->m_username);

    userinfo =std::make_unique<UserInfo>(this);

    ui->stackedWidget->addWidget(userinfo.get());

    connect(ui->treeWidget,&QTreeWidget::itemClicked,this,&AdminMainWindow::OnUserItemClicked);


    //   设置只允许最大化，不允许最小化
    this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |Qt::WindowCloseButtonHint);

    //显示窗口为最大化状态
    this->showMaximized();
}

AdminMainWindow::~AdminMainWindow()
{
    delete ui;
}

void AdminMainWindow::OnUserItemClicked(QTreeWidgetItem *item, int column)
{
    if(item->parent() && item->parent()->text(0)=="基础管理"&&item->text(0)=="用户")
    {
        ui->stackedWidget->setCurrentWidget(userinfo.get());

        //发送请求 请求用户信息
        QJsonObject obj;

        obj["role_id"]=3;

        Packet packet =Protocol::createPacket(UserInfoRequest,obj);

        QByteArray date =Protocol::serializePacket(packet);

        ClientSocket::instance()->senData(date);
    }
}

void AdminMainWindow::OnDoctorItemClicked(QTreeWidgetItem *item, int column)
{
    if(item->parent() && item->parent()->text(0)=="基础管理"&&item->text(0)=="医护人员")
    {
        ui->stackedWidget->setCurrentWidget(userinfo.get());

        //发送请求 请求用户信息
        QJsonObject obj;

        obj["role_id"]=2;

        Packet packet =Protocol::createPacket(UserInfoRequest,obj);

        QByteArray date =Protocol::serializePacket(packet);

        ClientSocket::instance()->senData(date);
    }
}

