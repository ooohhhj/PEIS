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

    ui->stackedWidget->addWidget(patientinfo.get());
    ui->stackedWidget->addWidget(inputmedicaexaminationdata.get());
    ui->stackedWidget->addWidget(healthexaminationreport.get());

    setDefaultWidget();

    connect(patientinfo.get(),&PatientInformation::onEditReportButtonClicked,this,&Nurse::on_EditReportButtonClicked);

    connect(patientinfo.get(),&PatientInformation::onLookCheckUpReportClicked,this,&Nurse::onLookCheckUpReportClicked);

    connect(healthexaminationreport.get(),&HealthExaminationReport::exitButtonClicked,this,&Nurse::exitButtonClicked);

    connect(inputmedicaexaminationdata.get(),&InputMedicaExaminationData::exitButtonClicked,this,&Nurse::InputMedicaExaminationData_exitButtonClicked);

    connect(patientinfo.get(),&PatientInformation::exitButtonClicked,this,&Nurse::setDefaultWidget);


    //   设置只允许最大化，不允许最小化
    this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |Qt::WindowCloseButtonHint);

    //显示窗口为最大化状态
    this->showMaximized();
}

Nurse::~Nurse()
{
    delete ui;
}

void Nurse::on_scheduleCheckupButton_clicked()
{
    //显示界面
    ui->stackedWidget->setCurrentWidget(patientinfo.get());
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

void Nurse::onLookCheckUpReportClicked(const QString &patientName, const QString &healthPackage, const QString &appointmentDate)
{
    ui->stackedWidget->setCurrentWidget(healthexaminationreport.get());

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

void Nurse::exitButtonClicked()
{
    ui->stackedWidget->setCurrentWidget(patientinfo.get());
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
    QPixmap image(":/pexels-algrey-Nurse .jpg");

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

