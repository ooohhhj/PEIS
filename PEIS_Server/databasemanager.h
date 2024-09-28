#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

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

    int getAppointmentCount(const QString& cardName,const QString &selectedDate);

    bool isUserAlreadyByAppointments(const QString& username,const QString &cardname,const QString &selectedDate);

    int getAvailablePackageCount(const QString& cardName,const QString&selecteDate);

    bool insertAppointment(const QString &username,const QString &itemName,const QString &appointmentDate);
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
