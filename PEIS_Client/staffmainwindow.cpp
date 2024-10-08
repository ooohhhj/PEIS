#include "staffmainwindow.h"
#include "ui_staffmainwindow.h"

StaffMainWindow::StaffMainWindow(QWidget *parent,const QString &username) :
    QMainWindow(parent),m_username(username),
    ui(new Ui::StaffMainWindow)
{
    ui->setupUi(this);

    ui->usernameLabel->setText(this->m_username);


    appointmentmanagement=std::make_unique<AppointmentManagement>(this);
    editmedicalexaminationreport =std::make_unique<EditMedicalExaminationReport>(this);

    ui->stackedWidget->addWidget(appointmentmanagement.get());
    ui->stackedWidget->addWidget(editmedicalexaminationreport.get());

    appointmentmanagement.get()->setUsername(m_username);


    //   设置只允许最大化，不允许最小化
    this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |Qt::WindowCloseButtonHint);

    //显示窗口为最大化状态
    this->showMaximized();

    connect(ClientSocket::instance(),&ClientSocket::OnPatientHealthExaminationReview,this,&StaffMainWindow::updatenoticeButton);

    connect(appointmentmanagement.get(),&AppointmentManagement::EditCheckuppreport,this,&StaffMainWindow::OnEditCheckuppreport);
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


void StaffMainWindow::on_noticeButton_clicked()
{
    QIcon icon(":/notice-No.png"); // 替换成你想要的图标路径
    ui->noticeButton->setIcon(icon);

}

void StaffMainWindow::updatenoticeButton()
{
    QIcon icon(":/notice-Yes.png"); // 替换成你想要的图标路径
    ui->noticeButton->setIcon(icon);
    qDebug()<<"来提醒";
}


void StaffMainWindow::on_checkupReportButton_clicked()
{




}

void StaffMainWindow::OnEditCheckuppreport(const QString &patientName, const QString &packageName, const QString &appointmentDate)
{
    ui->stackedWidget->setCurrentWidget(editmedicalexaminationreport.get());

    QJsonObject Obj;
    Obj["patientName"]=patientName;
    Obj["packageName"]=packageName;

    // 将获取的日期格式化为 YYYY-MM-DD
    Obj["appointmentDate"] = QDate::fromString(appointmentDate, "yyyy-M-d").toString("yyyy-MM-dd");
    Packet packet =Protocol::createPacket(EditCheckupReportRequest,Obj);

    QByteArray sendArray =Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(sendArray);
}

