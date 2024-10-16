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

