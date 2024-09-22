#include "clienthandler.h"


ClientHandler::ClientHandler(QTcpSocket *socket, QObject *parent) : QObject(parent), clientSocket(socket)
{

    // 设置此 runnable 对象在执行完成后自动删除
    setAutoDelete(true);
}

void ClientHandler::run()
{

    QByteArray data =clientSocket->readAll();
    qDebug() << "Received data from client:" << data;

    //反序列化
    Packet packet =Protocol::deserializePacket(data);

    //根据请求类型处理数据
    QByteArray result =processRequest(packet);

    emit requestProcessed(result);
}

QByteArray ClientHandler::processRequest(Packet &packet)
{

    //检查解包后的消息长度是否与包的长度匹配
    QJsonObject message = Protocol::parsePacket(packet);
    // 如果要检查长度，可以对 JSON 对象序列化后进行字节长度检查
    QByteArray jsonData = QJsonDocument(message).toJson(QJsonDocument::Compact);
    if(packet.length!=jsonData.size())
    {
        qDebug() << "数据包长度与实际消息内容不匹配: packet.length = "
                 << packet.length << ", message.size() = " << message.size();
        return QByteArray();  // 返回空的 QByteArray 代表错误或无效结果
    }
    qDebug()<<"we";
    switch (packet.requestType)
    {
    case UsernameIsExistRequest:
        return handleUsernameIsRequest(message);
        break;
    case RegisterRequest:
        return handleRegisterRequest(message);
        break;
    case PhoneNumberIsExistRequest:
        return handlePhoneNumberIsExistRequest(message);
        break;
    case ForGetPasswordRequest:
        return handleForgetPasswordRequest(message);
        break;
    case PhoneAndPasswordIsExistRequest:
        return handlePhoneAndPasswordIsExistRequest(message);
        break;
    case UsernameAndPasswordIsExistRequest:
        return handleUsernameAndPasswordIsExistRequest(message);
        break;
    case ReserveCheckupRequest:
        return handleReserveCheckupRequest(message);
        break;
    default:
        QString message =StatusMessage::InternalServerError;
        QJsonObject responseJson;
        responseJson["message"] = message;

        Packet packet =Protocol::createPacket(InternalServerError,responseJson);

        //序列化数据包
        QByteArray serializedResponse = Protocol::serializePacket(packet);

        return serializedResponse;
    }
}

QByteArray ClientHandler::handleRegisterRequest(const QJsonObject& registerDate)
{
    //处理注册请求

    //获取注册信息
    QString username = registerDate["username"].toString();
    QString sex = registerDate["sex"].toString();
    QString birthdate = registerDate["birthDate"].toString();
    QString idCard = registerDate["idNumber"].toString();
    QString address =registerDate["address"].toString();
    QString phone =registerDate["iphoneNumber"].toString();
    QString password =registerDate["password"].toString();

    qDebug() << "Register request received for username:" << username
             << " Gender:" << sex
             << " Birthdate:" << birthdate
             << " ID Card:" << idCard
             << " Address:" << address
             << " Phone:" << phone
             <<"password"<<password;

    // 注册请求的业务逻辑
    //判断用户是否存在
    QSqlQuery query = DatabaseManager::instance().findUserByName(username);

    QString responseMsg;
    int msgType ;
    if(query.next())
    {
        //存在 不能注册 发送消息给客户端
        responseMsg = StatusMessage::userAlreadyExists;
    }
    else
    {
        //不存在  将用户注册消息存储到数据库
        bool success =DatabaseManager::instance().insertUser(username,sex,birthdate,idCard,address,phone,password,3);


        if(success)
        {
            responseMsg = StatusMessage::RegisterationSuccessfully;
            msgType = RegisterSuccessfullyResponce;
        }
        else
        {
            responseMsg = StatusMessage::RegisterationFailed;
            msgType = RegisterFailedResponce;
        }
    }

    //将响应信息封装成JSON格式
    QJsonObject responseJson;
    responseJson["message"] =responseMsg;

    //将响应信息封装成数据包
    Packet packet =Protocol::createPacket(msgType,responseJson);

    //序列化数据包
    QByteArray serializedResponse =Protocol::serializePacket(packet);

    return serializedResponse;
}

QByteArray ClientHandler::handleUsernameIsRequest(const QJsonObject &usernameDate)
{
    //判断用户名是否存在

    //存在回复  不存在不回复
    QString username =usernameDate["username"].toString();

    QSqlQuery query = DatabaseManager::instance().findUserByName(username);

    QString responseMsg;

    if(query.next())
    {
        responseMsg = StatusMessage::userAlreadyExists;

        //将响应信息封装成JSON格式
        QJsonObject responseJson;
        responseJson["message"] =responseMsg;

        //将响应信息封装成数据包
        Packet packet =Protocol::createPacket(UsernameIsExistResponce,responseJson);

        //序列化数据包
        QByteArray serializedResponse =Protocol::serializePacket(packet);

        return serializedResponse;
    }
    else
    {
        QString message =StatusMessage::userNotExists;

        QJsonObject responseJson;
        responseJson["message"] = message;

        Packet packet =Protocol::createPacket(UsernameNotExistResponce,responseJson);

        //序列化数据包
        QByteArray serializedResponse = Protocol::serializePacket(packet);

        return serializedResponse;
    }

}

QByteArray ClientHandler::handlePhoneNumberIsExistRequest(const QJsonObject &phoneNumberDate)
{
    //获取手机号
    QString phoneNumber =phoneNumberDate["phoneNumber"].toString();

    bool success =DatabaseManager::instance().findUserByPhoneNumber(phoneNumber);
    int msgType;
    QString responceMsg;

    if(success)
    {
        responceMsg =StatusMessage::PhoneNumberExist;
        msgType = PhoneNumberIsExistResponce;
    }
    else
    {
        responceMsg = StatusMessage::PhoneNumberNotExist;
        msgType = PhoneNumberNotExistResponce;
    }

    QJsonObject responceJson;
    responceJson["message"] =responceMsg;

    Packet packet=Protocol::createPacket(msgType,responceJson);

    QByteArray serializedResponce =Protocol::serializePacket(packet);

    return serializedResponce;

}

QByteArray ClientHandler::handleForgetPasswordRequest(const QJsonObject &forgetPasswordDate)
{
    //获取数据
    QString phoneNumber =forgetPasswordDate["phoneNumber"].toString();
    QString password = forgetPasswordDate["password"].toString();

    //更新用户
    bool success =DatabaseManager::instance().updateUserPassword(phoneNumber,password);

    QString responceMsg ;
    int msgType;
    if(success)
    {
        responceMsg =StatusMessage::ForgetPasswordSuccessfully;
        msgType =ForgetPasswordSusseccfullyResponce;
    }
    else
    {
        responceMsg = StatusMessage::ForgetPasswordFailed;
        msgType = ForgetPasswordFailedResponce;
    }

    //将响应信息封装成JSOn格式

    QJsonObject responceJson;
    responceJson["message"] = responceMsg;


    //封包
    Packet packet =Protocol::createPacket(msgType,responceJson);

    //序列化
    QByteArray serializedResponse =Protocol::serializePacket(packet);

    return serializedResponse;
}

QByteArray ClientHandler::handlePhoneAndPasswordIsExistRequest(const QJsonObject &phoneAndPasswordDate)
{
    //获取数据
    QString phoneNumber =phoneAndPasswordDate["phoneNumber"].toString();
    QString password =phoneAndPasswordDate["password"].toString();

    qDebug()<<"phoneNumber"<<phoneNumber;
    qDebug()<<"password"<<password;
    bool success =DatabaseManager::instance().findUserByPhoneAndPassword(phoneNumber,password);

    QString responceMsg;
    QJsonObject responceJson;
    int msgType;
    if(success)
    {
        //手机号码与密码匹配
        //获取角色
        int roleId = DatabaseManager::instance().getRoleIdByPhoneNumber(phoneNumber);
        QString username =DatabaseManager::instance().getUsernameByPhoneNumber(phoneNumber);
        if(roleId!=-1 && username!=nullptr)
        {
            msgType =PhoneAndPasswordSuccessfullyResponce;
            responceJson["roleId"] = roleId;
            responceJson["username"]=username;
            qDebug()<<"roleId="<<roleId;
            qDebug()<<"username="<<username;
            responceMsg =StatusMessage::PhoneAndPasswordSuccessfully;
        }
        else
        {
            //查询失败
            msgType =InternalServerError;
            responceMsg =StatusMessage::InternalServerError;
        }
    }
    else
    {
        //不匹配
        msgType =USernameAndPasswordFailedResponce;
        responceMsg =StatusMessage::PhoneAndPasswordFailed;
    }


    responceJson["message"] =responceMsg;
    //封包
    Packet packet =Protocol::createPacket(msgType,responceJson);

    //序列化
    QByteArray serializedResponse =Protocol::serializePacket(packet);

    return serializedResponse;

}

QByteArray ClientHandler::handleUsernameAndPasswordIsExistRequest(const QJsonObject &usernameAndPasswordDate)
{
    //获取数据
    QString username =usernameAndPasswordDate["username"].toString();
    QString password =usernameAndPasswordDate["password"].toString();

    qDebug()<<"username"<<username;
    qDebug()<<"password"<<password;
    bool success =DatabaseManager::instance().findUserByUsernameAndPassword(username,password);

    QString responceMsg;
    QJsonObject responceJson;
    int msgType;
    if(success)
    {
        //手机号码与密码匹配
        //获取角色
        int roleId = DatabaseManager::instance().getRoleIdByUsername(username);
        if(roleId!=-1)
        {
            msgType =UsernameAndPasswordSuccessfullyResponce;
            responceJson["roleId"] = roleId;
            responceJson["username"]=username;
            qDebug()<<"roleId="<<roleId;
            qDebug()<<"username="<<username;
            responceMsg =StatusMessage::UsernameAndPasswordSuccessfully;
        }
        else
        {
            //查询失败
            msgType =InternalServerError;
            responceMsg =StatusMessage::InternalServerError;
        }
    }
    else
    {
        //不匹配
        msgType =USernameAndPasswordFailedResponce;
        responceMsg =StatusMessage::UsernameAndPasswordFailed;
    }


    responceJson["message"] =responceMsg;
    //封包
    Packet packet =Protocol::createPacket(msgType,responceJson);

    //序列化
    QByteArray serializedResponse =Protocol::serializePacket(packet);

    return serializedResponse;

}

QByteArray ClientHandler::handleReserveCheckupRequest(const QJsonObject &reserveCheckupDate)
{
    //获取页数
    int page =reserveCheckupDate["currentPage"].toInt();
    int itemsPerPage = reserveCheckupDate["itemsPerPage"].toInt();

    //计算机offset Limit
    int offset =(page-1)*itemsPerPage;

    //查询数据库  获取体检信息
    QSqlQuery query=DatabaseManager::instance().getReserveCheckup(itemsPerPage,offset);


    QJsonArray packagesArray;
    while(query.next())
    {
        QJsonObject packageObj;
        packageObj["package_name"] = query.value("package_name").toString();        // 套餐名称
        packageObj["target_population"] = query.value("target_population").toString(); // 使用人群
        packageObj["provider"] = query.value("provider").toString();                // 体检机构
        packageObj["price"] = query.value("price").toDouble();                      // 价格

        packagesArray.append(packageObj);
    }

    //将分页数据打包并发送给客户端
    QJsonObject responseObject;
    responseObject["packages"] =packagesArray;
    responseObject["totalPages"] =DatabaseManager::instance().calculateTotalPages(itemsPerPage);

    Packet responsePacket =Protocol::createPacket(ReserveCheckupResponce,responseObject);
    QByteArray serializedResponse =Protocol::serializePacket(responsePacket);
    return serializedResponse;


}




