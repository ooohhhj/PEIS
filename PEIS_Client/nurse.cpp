#include "nurse.h"
#include "ui_nurse.h"

Nurse::Nurse(QWidget *parent, const QString &username) :
    QMainWindow(parent),m_username(username),
    ui(new Ui::Nurse)
{
    ui->setupUi(this);
    ui->usernameLabel->setText(m_username);

    patientinfo =std::make_unique<PatientInformation>(this);
    inputmedicaexaminationdata =std::make_unique<InputMedicaExaminationData>(this);
    healthexaminationreport =std::make_unique<HealthExaminationReport>(this);
    appointmentsmanager_nurse =std::make_unique<AppointmentsManager_Nurse>(this);
    cancel_appointmentmanger = std::make_unique<Cancel_AppointmentManger>(this);

    ui->stackedWidget->addWidget(patientinfo.get());
    ui->stackedWidget->addWidget(inputmedicaexaminationdata.get());
    ui->stackedWidget->addWidget(healthexaminationreport.get());
    ui->stackedWidget->addWidget(appointmentsmanager_nurse.get());
    ui->stackedWidget->addWidget(cancel_appointmentmanger.get());

    setDefaultWidget();

    connect(patientinfo.get(),&PatientInformation::onEditReportButtonClicked,this,&Nurse::on_EditReportButtonClicked);

    connect(patientinfo.get(),&PatientInformation::onLookCheckUpReportClicked,this,&Nurse::onLookCheckUpReportClicked);

    connect(appointmentsmanager_nurse.get(),&AppointmentsManager_Nurse::appointmentsManager_Nurse_search,this,&Nurse::appointmentsManager_Nurse_search);

    connect(cancel_appointmentmanger.get(),&Cancel_AppointmentManger::onLookCheckUpReportClicked,this,&Nurse::onLookCheckUpReportClicked);

    connect(healthexaminationreport.get(),&HealthExaminationReport::exitButtonClicked,this,&Nurse::exitButtonClicked);

    connect(inputmedicaexaminationdata.get(),&InputMedicaExaminationData::exitButtonClicked,this,&Nurse::InputMedicaExaminationData_exitButtonClicked);


    connect(patientinfo.get(),&PatientInformation::exitButtonClicked,this,&Nurse::setDefaultWidget);
    connect(patientinfo.get(),&PatientInformation::exitButtonClicked,this, [this]() {
        buttonStyleSheet(ui->patientInfoButton);
    });

    connect(cancel_appointmentmanger.get(),&Cancel_AppointmentManger::exitButtonClicked,this,[this](){
       ui->stackedWidget->setCurrentWidget(appointmentsmanager_nurse.get());
    });

    connect(appointmentsmanager_nurse.get(),&AppointmentsManager_Nurse::exitButtonClicked,this,&Nurse::setDefaultWidget);
    connect(appointmentsmanager_nurse.get(),&AppointmentsManager_Nurse::exitButtonClicked,this, [this]() {
        buttonStyleSheet(ui->appointmentButton);
    });
    //   设置只允许最大化，不允许最小化
    this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |Qt::WindowCloseButtonHint);

    //显示窗口为最大化状态
    this->showMaximized();
}

Nurse::~Nurse()
{
    delete ui;
}

void Nurse::buttonStyleSheet(QPushButton *button)
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

void Nurse::on_EditReportButtonClicked(const QString &patientName, const QString &patientGender, const QString &patientPhone, const QString &patientBirthDate, const QString &healthPackage, const QString &appointmentDate, const QString &appointmentStatus)
{

    ui->stackedWidget->setCurrentWidget(inputmedicaexaminationdata.get());

    inputmedicaexaminationdata.get()->setPatientInfo(patientName,patientGender,patientPhone,patientBirthDate,healthPackage,appointmentDate,appointmentStatus);

    //发送请求体检项目请求
    QJsonObject obj;
    obj["packageName"]=healthPackage;
    obj["patientName"] =patientName;
    obj["appointmentDate"] =appointmentDate;

    Packet packet =Protocol::createPacket(HealthCheckupItemRequest,obj);

    QByteArray sendArray =Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(sendArray);

}

void Nurse::onLookCheckUpReportClicked(const QString &patientName, const QString &healthPackage, const QString &appointmentDate, const int &flag)
{
    ui->stackedWidget->setCurrentWidget(healthexaminationreport.get());
    healthexaminationreport.get()->setSignalFlag(flag);

    //发送记录申请
    QJsonObject Info;

    Info["patientName"] =patientName;
    Info["packageName"] =healthPackage;
    Info["appointmentDate"] =appointmentDate;

    //封包
    Packet reserveCheckupPacket =Protocol::createPacket(GetHealthExaminationRePortListRequest_Nurse,Info);

    //序列化
    QByteArray dataToSend = Protocol::serializePacket(reserveCheckupPacket);

    ClientSocket::instance()->senData(dataToSend);
}

void Nurse::exitButtonClicked(const int &flag)
{
    if(flag == 1)
    {
        ui->stackedWidget->setCurrentWidget(patientinfo.get());
    }
    else if(flag == 2)
    {
        ui->stackedWidget->setCurrentWidget(cancel_appointmentmanger.get());
    }
}

void Nurse::InputMedicaExaminationData_exitButtonClicked()
{
    ui->stackedWidget->setCurrentWidget(patientinfo.get());
}

void Nurse::setDefaultWidget()
{
    QWidget *centralWidget = new QWidget(this);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QLabel *imageLabel = new QLabel(this);
    QPixmap image(":/pexels-ron-lach-Nurse.jpg");

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


void Nurse::on_patientInfoButton_clicked()
{
    //显示界面
    ui->stackedWidget->setCurrentWidget(patientinfo.get());

    ui->patientInfoButton->setStyleSheet(
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
    ui->patientInfoButton->setCheckable(true);

    // 设置按钮为选中状态
    ui->patientInfoButton->setChecked(true);
}


void Nurse::on_appointmentButton_clicked()
{
    //显示界面
    ui->stackedWidget->setCurrentWidget(appointmentsmanager_nurse.get());

    ui->appointmentButton->setStyleSheet(
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
    ui->appointmentButton->setCheckable(true);

    // 设置按钮为选中状态
    ui->appointmentButton->setChecked(true);
}

void Nurse::appointmentsManager_Nurse_search(const QString&patientName)
{
    //显示界面
    ui->stackedWidget->setCurrentWidget(cancel_appointmentmanger.get());

    cancel_appointmentmanger.get()->setPatientName(patientName);
}


