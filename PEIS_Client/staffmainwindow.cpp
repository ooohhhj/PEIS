#include "staffmainwindow.h"
#include "ui_staffmainwindow.h"

StaffMainWindow::StaffMainWindow(QWidget *parent,const QString &username) :
    QMainWindow(parent),m_username(username),
    ui(new Ui::StaffMainWindow)
{
    ui->setupUi(this);

    ui->usernameLabel->setText(this->m_username);


    appointmentmanagement=std::make_unique<AppointmentManagement>(this);

    ui->stackedWidget->addWidget(appointmentmanagement.get());

    appointmentmanagement.get()->setUsername(m_username);





    //   设置只允许最大化，不允许最小化
    this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |Qt::WindowCloseButtonHint);

    //显示窗口为最大化状态
    this->showMaximized();
}

StaffMainWindow::~StaffMainWindow()
{
    delete ui;
}

void StaffMainWindow::OnStartDateRequest()
{
    QJsonObject Dateobj;
    Dateobj["username"] =m_username;

    Packet packet =Protocol::createPacket(StartDateRequest,Dateobj);

    QByteArray sendArray =Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(sendArray);
}


void StaffMainWindow::on_healthCheckRecordButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(appointmentmanagement.get());
    QJsonObject appointmentObj;
    appointmentObj["username"]=m_username;

    Packet packet =Protocol::createPacket(AppointmentInformationRequest,appointmentObj);

    QByteArray sendArray =Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(sendArray);
}

