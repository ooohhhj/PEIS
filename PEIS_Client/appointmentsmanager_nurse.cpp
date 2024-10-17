#include "appointmentsmanager_nurse.h"
#include "ui_appointmentsmanager_nurse.h"

AppointmentsManager_Nurse::AppointmentsManager_Nurse(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppointmentsManager_Nurse)
{
    ui->setupUi(this);



}

AppointmentsManager_Nurse::~AppointmentsManager_Nurse()
{
    delete ui;
}

void AppointmentsManager_Nurse::on_searchButton_clicked()
{
    //获取输入框
    QString patientName = ui->patientNameLineEdit->text();

    if(patientName.isEmpty())
    {
        ClientSocket::instance()->showMessageBox(":/warning.png","预约管理","输入信息为空");
        return ;
    }

    QJsonObject obj ;
    obj["patientName"] =patientName;

    qDebug()<<"patientName="<<patientName;

    Packet packet = Protocol::createPacket(PatientInfoRequest_Nurse,obj);

    QByteArray array =Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(array);

    emit appointmentsManager_Nurse_search(patientName);
}



void AppointmentsManager_Nurse::on_returnExitButton_clicked()
{
    emit exitButtonClicked();
}

