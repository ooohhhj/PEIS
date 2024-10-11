#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QDate>

#include <QCryptographicHash>
#include <QRandomGenerator>

class DatabaseManager
{
public:
    DatabaseManager();

    //使用单例模式
    static DatabaseManager & instance();

    // 删除拷贝构造函数和赋值操作符，防止对象被拷贝
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    ~DatabaseManager();

    //插入用户信息
    bool insertUser(const QString &username,const QString &gender,const QString &birthdate,const QString &idCard,const QString &address,const QString &phoneNumber,const QString &password,const int &role_id);

    //更新用户密码
    bool updateUserPassword(const QString &phoneNumber,const QString &password);

    //根据用户名查找用户
    QSqlQuery findUserByName(const QString &username);

    //判断手机号是否存在
    bool findUserByPhoneNumber(const QString &phoneNumber);

    bool findUserByPhoneAndPassword(const QString &phoneNumber,const QString &password);

    bool findUserByUsernameAndPassword(const QString &username,const QString &password);

    QString getSaltByUsername(const QString &username);

    QString getSaltByPhoneNumber(const QString &phoneNumber);

    int getRoleIdByPhoneNumber(const QString &phoneNumber);

    int getRoleIdByUsername(const QString &username);

    QString getUsernameByPhoneNumber(const QString &phoneNumber);

    QSqlQuery getReserveCheckup(const int& itemsPerPage,const int& offset);

    QSqlQuery getPackageNameInfo(const QString &packageName);

    QSqlQuery searchPackageNameInfo(const QString & searchPackageName, int offset, int itemsPerPage);

    int calculateTotalPages(int itemsPerPage);

    int getPackageCount(const QString &packageName);

    int getAppointmentCount(const int& packageId,const QString &selectedDate);

    bool isUserAlreadyByAppointments(const int &userId, const int &packageId, const QString &appointmentDate);

    int getAvailablePackageCount(const QString& cardName,const QString&selecteDate);

    bool insertAppointment(const int &userId, const int &packageId,const QString &appointmentDate);

    int getdoctorIdByDoctorTable(const int&userId);

    int getUserIdByUsername(const QString &username);//传入用户名获取用户id
    int getPackageIdByName(const QString &packageName);//传入套餐名字获取套餐id
    int getDoctorByDepartment(const int &packageId);
    QString getUsernameByuserId(const int & userid);
    QString getPackageNameBypackageId(const int & packageId);


    int getDoctorIdByAppointments(const int & patientId,const int & packageId,const QString & appointmentDate);

    bool InsertHealthCheckData( QJsonObject &healthcheckupDate);

    bool UpdateHealthCheckData( QJsonObject &healthcheckupDate);

    bool isExistCheckupDate(const QString & packageName,const QString & patientName,const QString & appointmentDate);

    bool updateStatusByAppointment(const int & patientId,const int & packageId,const QString & appointmentDate);

    QSqlQuery getAppointmentsByusername(const QString & username);

    QString getStartDateByusername(const QString &username);

    QSqlQuery getUserInfoByUsername(const QString&username);

    QSqlQuery getPackageItemInfo(const QString &packagename);

    QSqlQuery getRecordHealthCheckupDate(const QString & patientName);

    QSqlQuery getAppointmentInfoByusername(const QString & username);

    QSqlQuery getPatientCheckupDate(const QString& patientName,const QString &packageName,const QString & appointmentDate);

    bool updatePatientReport(const QJsonObject &reportDateObj );

    QList<QVariantMap> GetHealthExaminationPatientInfo(const int &doctorId);

    QList<QVariantMap> GetPatientHealthExaminationData(const int &healthCheckupId);

    bool CancelAppointment(const QString &username, const QString &packageName, const QString &appointmentDate);

    bool updateReportPath(const QString& patientName,const QString & packageName,const QString &appointmentDate,const QString& reportPath);

    bool updateAppointMents(const QString& patientName,const QString & packageName,const QString &appointmentDate);

    int getPatientIdByName(const QString &patientName);

    QString getUserReport(const QString& username,const QString & packageName,const QString &appointmentDate);

    //检查数据库连接状态
    bool isConnected() const;

    QSqlQuery getUserInfosByRoleId(const int&role_id);

    //生成盐值
    QString generateSalt(int length );

    //对密码进行哈希加密
    QString hashPassword(const QString &password,const QString &salt);


private:
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
