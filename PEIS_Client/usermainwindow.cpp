#include "usermainwindow.h"
#include "ui_usermainwindow.h"


UserMainWindow::UserMainWindow(QWidget *parent,const QString &username) :
    QMainWindow(parent),m_username(username),
    ui(new Ui::UserMainWindow)
{
    ui->setupUi(this);

    ui->usernameLabel->setText(this->m_username);
    qDebug()<<"username="<<this->m_username;


   ui->stackedWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


    //添加预约体检界面到StackWidget中
    scheduleCheckUp =std::make_unique<ScheduleCheckup>(this);
    ui->stackedWidget->addWidget(scheduleCheckUp.get());


    //   设置只允许最大化，不允许最小化
    this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |Qt::WindowCloseButtonHint);

    //显示窗口为最大化状态
    this->showMaximized();


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
}

