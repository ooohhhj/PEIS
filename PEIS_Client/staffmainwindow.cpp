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
    electronicmedicalrecord =std::make_unique<ElectronicMedicalRecord>(this);
    healthexaminationreport =std::make_unique<HealthExaminationReport>(this);


    ui->stackedWidget->addWidget(appointmentmanagement.get());
    ui->stackedWidget->addWidget(editmedicalexaminationreport.get());
    ui->stackedWidget->addWidget(electronicmedicalrecord.get());
    ui->stackedWidget->addWidget(healthexaminationreport.get());

    appointmentmanagement.get()->setUsername(m_username);
    electronicmedicalrecord.get()->setUsername(m_username);


    //   设置只允许最大化，不允许最小化
    this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |Qt::WindowCloseButtonHint);

    //显示窗口为最大化状态
    this->showMaximized();

    connect(ClientSocket::instance(),&ClientSocket::OnPatientHealthExaminationReview,this,&StaffMainWindow::updatenoticeButton);

    connect(appointmentmanagement.get(),&AppointmentManagement::EditCheckuppreport,this,&StaffMainWindow::OnEditCheckuppreport);

    connect(electronicmedicalrecord.get(),&ElectronicMedicalRecord::onLookCheckUpReportClicked,this,&StaffMainWindow::onLookCheckUpReportClicked);

    connect(appointmentmanagement.get(),&AppointmentManagement::onLookCheckUpReportClicked,this,&StaffMainWindow::onLookCheckUpReportClicked);

    connect(healthexaminationreport.get(),&HealthExaminationReport::exitButtonClicked,this,&StaffMainWindow::healthexaminationreport_exitButtonClicked);

    connect(appointmentmanagement.get(),&AppointmentManagement::exitButtonClicked,this,&StaffMainWindow::setDefaultWidget);

    connect(editmedicalexaminationreport.get(),&EditMedicalExaminationReport::exitButtonClicked,this,&StaffMainWindow::editmedicalexaminationreport_exitButtonClicked);

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

void StaffMainWindow::onLookCheckUpReportClicked(const QString &patientName, const QString &healthPackage, const QString &appointmentDate, const int &flag)
{
    ui->stackedWidget->setCurrentWidget(healthexaminationreport.get());
    healthexaminationreport.get()->setSignalFlag(flag);
    //发送记录申请
    QJsonObject Info;

    Info["patientName"] =patientName;
    Info["packageName"] =healthPackage;
    Info["appointmentDate"] =appointmentDate;

    //封包
    Packet reserveCheckupPacket =Protocol::createPacket(GetHealthExaminationRePortListRequest_doctor,Info);

    //序列化
    QByteArray dataToSend = Protocol::serializePacket(reserveCheckupPacket);

    ClientSocket::instance()->senData(dataToSend);
}

void StaffMainWindow::setDefaultWidget()
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

void StaffMainWindow::healthexaminationreport_exitButtonClicked(const int &flag)
{
    if(flag==1)
    {
        ui->stackedWidget->setCurrentWidget(appointmentmanagement.get());
    }
    else if(flag ==2)
    {
        ui->stackedWidget->setCurrentWidget(electronicmedicalrecord.get());
    }
}

void StaffMainWindow::editmedicalexaminationreport_exitButtonClicked()
{
    ui->stackedWidget->setCurrentWidget(editmedicalexaminationreport.get());
}

void StaffMainWindow::on_checkupReportButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(electronicmedicalrecord.get());
    QJsonObject appointmentObj;
    appointmentObj["username"]=m_username;

    Packet packet =Protocol::createPacket(ElectronicMedicalRecordRequest,appointmentObj);

    QByteArray sendArray =Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(sendArray);
}

