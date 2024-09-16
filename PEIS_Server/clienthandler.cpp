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

    //解包
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
    switch (packet.requestType)
    {
    case UsernameIsExistRequest:
        return handleUsernameIsRequest(message);
        break;
    case RegisterRequest:
        return handleRegisterRequest(message);
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
            responseMsg = StatusMessage::RegisterationSuccessful;
        }
        else
        {
            responseMsg = StatusMessage::RegisterationFailed;
        }
    }

    //将响应信息封装成JSON格式
    QJsonObject responseJson;
    responseJson["message"] =responseMsg;

    //将响应信息封装成数据包
    Packet packet =Protocol::createPacket(RegisterResponce,responseJson);

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


