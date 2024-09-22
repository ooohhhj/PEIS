#ifndef PROTOCOL_H
#define PROTOCOL_H


#include<QString>
#include<QJsonDocument>
#include<QJsonObject>
#include<QByteArray>
#include<QString>
#include <QObject>
#include <QDebug>

///定义数据包结构
struct Packet
{
    int requestType;                           //请求类型
    int length;                                //消息长度
    QByteArray message;                        //消息内容(JSON格式)
};

//枚举消息类型
enum RequestType
{
    UsernameIsExistRequest =1,                  //用户名是否存在请求
    RegisterRequest ,                           //注册请求
    PhoneNumberIsExistRequest,                  //手机号是否存在请求
    ForGetPasswordRequest,                      //找回密码请求
    PhoneAndPasswordIsExistRequest,             //手机号码与密码是否匹配
    UsernameAndPasswordIsExistRequest,          //用户名与密码是否匹配
    ReserveCheckupRequest,                        // 预约体检信息请求

    UpdateCheckupPackageRequest                  // 更新体检套餐请求

};

enum ResponseType
{
    UsernameIsExistResponce = 1,                 //用户名存在回复
    UsernameNotExistResponce,                   //用户名不存在回复

    RegisterSuccessfullyResponce ,              //注册成功回复
    RegisterFailedResponce,                     //注册失败

    PhoneNumberIsExistResponce ,                //手机号存在
    PhoneNumberNotExistResponce,                //手机号不存在

    ForgetPasswordSusseccfullyResponce,         //找回密码成功
    ForgetPasswordFailedResponce,               //找回密码失败

    PhoneAndPasswordSuccessfullyResponce,       //手机号码与密码匹配成功
    PhoneAndPasswordFailedResponce,             //手机号码与密码不匹配

    UsernameAndPasswordSuccessfullyResponce,    //用户名与密码匹配成功
    USernameAndPasswordFailedResponce,          //用户名与密码不匹配

     ReserveCheckupResponce,                     //体检套餐回复

    InternalServerError,                        //未知请求

};

namespace StatusMessage
{

  const QString userAlreadyExists = "用户名已存在";
  const QString userNotExists = "用户名不存在";

  const QString RegisterationFailed ="注册失败";
  const QString RegisterationSuccessfully ="注册成功";

  const QString PhoneNumberExist ="手机号存在";
  const QString PhoneNumberNotExist ="手机号不存在";

  const QString ForgetPasswordSuccessfully ="找回密码成功";
  const QString ForgetPasswordFailed ="找回密码失败";

  const QString PhoneAndPasswordSuccessfully ="手机号码与密码匹配";
  const QString PhoneAndPasswordFailed ="手机号码或密码输入有误";

  const QString UsernameAndPasswordSuccessfully ="用户名与密码匹配";
  const QString UsernameAndPasswordFailed ="用户名或密码输入有误";

  const QString InternalServerError = "内部错误";
};

class Protocol:public QObject
{
    Q_OBJECT
public:
    Protocol(QObject*parent =nullptr):QObject(parent){};

    //创建数据包
    static Packet createPacket(int requestType,const QJsonObject&jsonMessage);

    //解包
    static QJsonObject parsePacket(const Packet& packet);


    //序列化数据包
    static QByteArray serializePacket(const Packet& packet);


    //反序列化接收到的数据
    static Packet deserializePacket(const QByteArray&data);

};

#endif // PROTOCOL_H