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


    appointmentmanagement.get()->setUsername(m_username);
    electronicmedicalrecord.get()->setUsername(m_username);


    setDefaultWidget();

    ui->stackedWidget->addWidget(appointmentmanagement.get());
    ui->stackedWidget->addWidget(editmedicalexaminationreport.get());
    ui->stackedWidget->addWidget(electronicmedicalrecord.get());
    ui->stackedWidget->addWidget(healthexaminationreport.get());


    //   设置只允许最大化，不允许最小化
    this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |Qt::WindowCloseButtonHint);

    //显示窗口为最大化状态
    this->showMaximized();

    connect(ClientSocket::instance(),&ClientSocket::OnPatientHealthExaminationReview,this,&StaffMainWindow::updatenoticeButton);

    connect(appointmentmanagement.get(),&AppointmentManagement::EditCheckuppreport,this,&StaffMainWindow::OnEditCheckuppreport);

    connect(electronicmedicalrecord.get(),&ElectronicMedicalRecord::onLookCheckUpReportClicked,this,&StaffMainWindow::onLookCheckUpReportClicked);
    connect(appointmentmanagement.get(),&AppointmentManagement::onLookCheckUpReportClicked,this,&StaffMainWindow::onLookCheckUpReportClicked);

    connect(appointmentmanagement.get(),&AppointmentManagement::exitButtonClicked,this,&StaffMainWindow::setDefaultWidget);
    connect(appointmentmanagement.get(), &AppointmentManagement::exitButtonClicked, this, [this]() {
        buttonStyleSheet(ui->AppointmentManagerButton);
    });

    connect(electronicmedicalrecord.get(),&ElectronicMedicalRecord::exitButtonClicked,this,&StaffMainWindow::setDefaultWidget);
    connect(electronicmedicalrecord.get(), &ElectronicMedicalRecord::exitButtonClicked, this, [this]() {
        buttonStyleSheet(ui->electronicmedicalrecordButton);
    });

    connect(healthexaminationreport.get(),&HealthExaminationReport::exitButtonClicked,this,&StaffMainWindow::healthexaminationreport_exitButtonClicked);
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

void StaffMainWindow::buttonStyleSheet(QPushButton *button)
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

    button->setStyleSheet(buttonStyleSheet);
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
    QWidget *centralWidget = new QWidget(this);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QLabel *imageLabel = new QLabel(this);
    QPixmap image(":/pexels-algrey-doctor.jpg");

    if (!image.isNull()) {
        imageLabel->setPixmap(image);
        imageLabel->setScaledContents(true);
    } else {
        qDebug() << "Image not loaded.";
    }

    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    layout->addWidget(imageLabel);
    centralWidget->setLayout(layout);

    // 添加并设置为当前显示的部件
    ui->stackedWidget->addWidget(centralWidget);
    ui->stackedWidget->setCurrentWidget(centralWidget);

    // 强制刷新 stackedWidget
    ui->stackedWidget->update();
    ui->stackedWidget->repaint();
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


void StaffMainWindow::on_AppointmentManagerButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(appointmentmanagement.get());

    ui->AppointmentManagerButton->setStyleSheet(
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
    ui->AppointmentManagerButton->setCheckable(true);

    // 设置按钮为选中状态
    ui->AppointmentManagerButton->setChecked(true);

    QJsonObject appointmentObj;
    appointmentObj["username"]=m_username;

    Packet packet =Protocol::createPacket(AppointmentInformationRequest,appointmentObj);

    QByteArray sendArray =Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(sendArray);
}


void StaffMainWindow::on_electronicmedicalrecordButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(electronicmedicalrecord.get());

    ui->electronicmedicalrecordButton->setStyleSheet(
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
    ui->electronicmedicalrecordButton->setCheckable(true);

    // 设置按钮为选中状态
    ui->electronicmedicalrecordButton->setChecked(true);

    QJsonObject appointmentObj;
    appointmentObj["username"]=m_username;

    Packet packet =Protocol::createPacket(ElectronicMedicalRecordRequest,appointmentObj);

    QByteArray sendArray =Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(sendArray);
}

