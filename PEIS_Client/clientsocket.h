#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QIcon>
#include <QGridLayout>
#include <QLabel>
#include <QJsonArray>
#include <QDateTime>

#include "protocol.h"

class ClientSocket : public QObject
{
    Q_OBJECT
public:
    explicit ClientSocket(QObject *parent = nullptr);

    //使用单例模式 获取ClientSocket实例
    static ClientSocket* instance();


    //禁止复制和赋值操作
    ClientSocket(const ClientSocket&) =delete;
    ClientSocket& operator=(const ClientSocket&) =delete;

    void connectToServer(const QString & host,quint16 port);

    void senData(const QByteArray&data);

    void processResponse(Packet &packet);

    void showMessageBox(const QString &iconPath, const QString &windowsTitle,const QString &message);

    QString convertAppointmentDate(const QString &inputDate);


private slots:
    void readData();
signals:
    void usernameIsExist(const QString&message);
    void RegisterSuccessfully();
    void ForgetPasswordSuccessfully();
    void PhoneNumberNotExist(const QString & message);

    void Logined(const int &roleId,const QString &username);

    void ReserveCheckup(const QJsonArray & jsonArray,const int& totalPage);
    void PackageNameInfo(const QJsonArray & jsonArray,const QString &packageDescription);

    void updateUserAppointment(const QString& selectdate,const QString & dateText);

    void userInfos(const QJsonArray &jsonArray);

    void OnstartDate(const QString & startDate);

    void OnAppointmentsDate(const QJsonArray& appointmentsArray);

    void OnPatientInfoResponce(const QJsonArray& patientInfoArray);

    void OnHealthCheckupItemResponce(const QJsonArray & packageItems);

    void OnRecordHealthCheckupResponce(const QJsonArray & checkUpDate);

    void onGetAppointmentInfo(const QString &username,const QString &packageName,const QString &appointmentDate,const QString&status);

    void OnPatientHealthExaminationReview();

    void OnPatientHealthExaminationDateResponce(const QJsonObject& patientDateInfoObject);

    void OnUserCheckupGenerateNotice();
private:
    QTcpSocket * tcpSocket;

};

#endif // CLIENTSOCKET_H
