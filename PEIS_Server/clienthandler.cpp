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
    case UpdateCheckupPackageRequest:
        return handleReserveCheckupRequest(message);
        break;
    case PackageInformationRequest:
        return handlePackageInformationRequest(message);
        break;
    case SearchPackageRequest:
        return handleSearchPackageRequest(message);
        break;
    case UpdateSearchPackageRequest:
        return handleSearchPackageRequest(message);
        break;
    case GetCheckupPackageCountRequest:
        return handleGetCheckupPackageCountRequest(message);
        break;
    case UserInfoRequest:
        return handleUserInfoRequest(message);
        break;
    case StartDateRequest:
        return handleStartDateRequest(message);
        break;
    case AppointmentInformationRequest:
        return handleAppointmentInformationRequest(message);
        break;
    case PatientInfoRequest:
        return handlePatientInfoRequest(message);
        break;
    case HealthCheckupItemRequest:
        return handleHealthCheckupItemRequest(message);
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
        bool success =DatabaseManager::instance().insertUser(username,sex,birthdate,idCard,address,phone,password,4);


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

QByteArray ClientHandler::handlePackageInformationRequest(const QJsonObject &packageNameDate)
{
    QString packageName =packageNameDate["packageName"].toString();

    //查询数据库  获取套餐信息
    QSqlQuery query =DatabaseManager::instance().getPackageNameInfo(packageName);

    QJsonArray packageNameInfoArray;
    QString packageDescription; // 用于存储套餐描述

    if (query.next()) {
        packageDescription = query.value("package_description").toString(); // 获取套餐描述

        // 继续处理项目
        do {
            QJsonObject packageObj;
            packageObj["item_name"] = query.value("item_name").toString();
            packageObj["item_description"] = query.value("item_description").toString();
            packageNameInfoArray.append(packageObj);
        } while (query.next());
    }

    QJsonObject responseObject;
    responseObject["package_description"] = packageDescription; // 使用获取的描述
    responseObject["packageInfo"] = packageNameInfoArray;

    Packet responsePacket = Protocol::createPacket(PackageInformationResponce, responseObject);
    QByteArray serializedResponse = Protocol::serializePacket(responsePacket);
    return serializedResponse;
}

QByteArray ClientHandler::handleSearchPackageRequest(const QJsonObject &searchPackageDate)
{
    QString searchPackage = searchPackageDate["searchLineEdit"].toString();
    int page = searchPackageDate.contains("page") ? searchPackageDate["page"].toInt() : 1;

    int itemsPerPage = 3; // 每页显示的套餐数量
    int offset = (page - 1) * itemsPerPage; // 计算偏移量


    // 查询套餐总数量
    int totalCount = DatabaseManager::instance().getPackageCount(searchPackage);
    int totalPages = (totalCount + itemsPerPage - 1) / itemsPerPage; // 计算总页数

    QSqlQuery query = DatabaseManager::instance().searchPackageNameInfo(searchPackage, offset, itemsPerPage);

    QJsonArray packageNameInfoArray;

    while(query.next())
    {
        QJsonObject packageObj;
        packageObj["package_name"] = query.value("package_name").toString();        // 套餐名称
        packageObj["target_population"] = query.value("target_population").toString(); // 使用人群
        packageObj["provider"] = query.value("provider").toString();                // 体检机构
        packageObj["price"] = query.value("price").toDouble();                      // 价格

        packageNameInfoArray.append(packageObj);
    }

    QJsonObject responseObject;
    responseObject["packages"] = packageNameInfoArray;
    responseObject["totalPages"] = totalPages; // 返回总页数


    Packet responsePacket = Protocol::createPacket(SearchPackageResponce, responseObject);
    QByteArray serializedResponse = Protocol::serializePacket(responsePacket);

    return serializedResponse;
}

QByteArray ClientHandler::handleGetCheckupPackageCountRequest(const QJsonObject &checkupPackageDate)
{
    QString username =checkupPackageDate["username"].toString();
    QString cardName =checkupPackageDate["cardName"].toString();
    QString selectDate =checkupPackageDate["selectDate"].toString();


    QJsonObject responseObject;
    QString message ;

    int userId =DatabaseManager::instance().getUserIdByUsername(username);
    int packageId =DatabaseManager::instance().getPackageIdByName(cardName);
    //先判断用户是否预约
    if(DatabaseManager::instance().isUserAlreadyByAppointments(userId,packageId,selectDate))
    {

        message =StatusMessage::UserAppointmentConfirmed;
    }
    else
    {
        //未预约
        //获取套餐数量  获取预约表的套餐数量比较
        if(DatabaseManager::instance().getAppointmentCount(packageId,selectDate) <
                DatabaseManager::instance().getAvailablePackageCount(cardName,selectDate))
        {
            //说明还可以预约
            //创建预约
            if(DatabaseManager::instance().insertAppointment(userId,packageId,selectDate))
            {
                message =StatusMessage::AppointmentSuccessful;
                updateUserAppointments(selectDate);

            }
            else
            {
                message =StatusMessage::InternalServerError;
            }
        }
        else
        {
            //不能预约
            message =StatusMessage::CannotMakeanAppointment;
        }
    }

    responseObject["message"] = message;
    qDebug()<<message;

    Packet packet =Protocol::createPacket(GetCheckupPackageCountResponce,responseObject);
    QByteArray serializedResponse = Protocol::serializePacket(packet);

    return serializedResponse;


}

QByteArray ClientHandler::handleUserInfoRequest(const QJsonObject &role_idDate)
{
    int role_id = role_idDate["role_id"].toInt();

    QSqlQuery query = DatabaseManager::instance().getUserInfosByRoleId(role_id);

    QJsonArray userArray;

    // 将查询结果填充到 JSON 数组中
    while (query.next()) {
        QJsonObject userObject;

        userObject["id"] = query.value("id").toInt();
        userObject["username"] = query.value("username").toString();
        userObject["gender"] = query.value("gender").toString();
        userObject["birth_date"] = query.value("birth_date").toString();
        userObject["id_card"] = query.value("id_card").toString();
        userObject["address"] = query.value("address").toString();
        userObject["phone_number"] = query.value("phone_number").toString();
        userObject["role"] = query.value("role_name").toString();
        userObject["created_at"] = query.value("created_at").toString();
        userObject["updated_at"] = query.value("updated_at").toString();

        userArray.append(userObject);
    }

    QJsonObject obj;
    obj["userInfos"]=userArray;

    Packet packet =Protocol::createPacket(UserInfoResponce,obj);

    QByteArray array =Protocol::serializePacket(packet);

    return array;

}

QByteArray ClientHandler::handleStartDateRequest(const QJsonObject &usernameDate)
{
    QString username = usernameDate["username"].toString();

    QString startDate =DatabaseManager::instance().getStartDateByusername(username);

    QJsonObject obj;

    obj["startDate"]=startDate;

    Packet packet = Protocol::createPacket(StartDateResponce,obj);

    QByteArray array =Protocol::serializePacket(packet);

    return array;

}

QByteArray ClientHandler::handleAppointmentInformationRequest(const QJsonObject &usernameDate)
{
    QString username = usernameDate["username"].toString();

    QSqlQuery query =DatabaseManager::instance().getAppointmentsByusername(username);

    // 如果查询失败，返回错误信息
    if (!query.isActive() || !query.isSelect()) {
        QJsonObject errorResponse;
        errorResponse["status"] = "error";
        errorResponse["message"] = "Failed to retrieve appointment information.";
        QJsonDocument errorDoc(errorResponse);
        return QByteArray();
    }

    // 准备一个 JSON 数组存储所有预约信息
    QJsonArray appointmentsArray;

    // 遍历查询结果
    while (query.next()) {
        QJsonObject appointment;

        // 提取每个预约的字段信息
        QString patientName = query.value("patient_name").toString();
        QString patientPhone = query.value("patient_phone").toString();
        QString healthPackage = query.value("health_package").toString();
        QDate appointmentDate = query.value("appointment_date").toDate();
        QString appointmentStatus = query.value("appointment_status").toString();


        // 将每条预约信息存入 JSON 对象
        appointment["patient_name"] = patientName;
        appointment["patient_phone"] = patientPhone;
        appointment["health_package"] = healthPackage;
        appointment["appointment_date"] = appointmentDate.toString(Qt::ISODate);
        appointment["appointment_status"] = appointmentStatus;

        // 将该预约信息添加到 JSON 数组中
        appointmentsArray.append(appointment);
    }

    QJsonObject appointmentInfo;
    appointmentInfo["appointments"] =appointmentsArray;

    Packet packet =Protocol::createPacket(AppointmentInformationResponce,appointmentInfo);

    QByteArray array =Protocol::serializePacket(packet);

    return array;

}

QByteArray ClientHandler::handlePatientInfoRequest(const QJsonObject &patienNameDate)
{
    QString patientName = patienNameDate["patientName"].toString();

    QSqlQuery query =DatabaseManager::instance().getUserInfoByUsername(patientName);


    QJsonArray patientInfoArray;
    // 处理查询结果
    while (query.next()) {

        QJsonObject patientInfo;
        QString patientName = query.value("patient_name").toString();
        QString patientGender = query.value("patient_gender").toString();
        QString patientPhone = query.value("patient_phone").toString();
        QDate patientBirthDate = query.value("patient_birth_date").toDate();
        QString healthPackage = query.value("health_package").toString();
        QDateTime appointmentDate = query.value("appointment_date").toDateTime();
        QString appointmentStatus = query.value("appointment_status").toString();

        patientInfo["patientName"]=patientName;
        patientInfo["patientGender"]=patientGender;
        patientInfo["patientPhone"]=patientPhone;
        patientInfo["patientBirthDate"]=patientBirthDate.toString();
        patientInfo["healthPackage"]=healthPackage;
        patientInfo["appointmentDate"]=appointmentDate.toString();
        patientInfo["appointmentStatus"]=appointmentStatus;

        // 将该预约信息添加到 JSON 数组中
        patientInfoArray.append(patientInfo);

    }

    QJsonObject patientInfo;;
    patientInfo["patientInfo"] =patientInfoArray;

    Packet packet =Protocol::createPacket(PatientInfoResponce,patientInfo);

    QByteArray array =Protocol::serializePacket(packet);

    return array;
}

QByteArray ClientHandler::handleHealthCheckupItemRequest(const QJsonObject &packageName)
{
    QString package =packageName["packageName"].toString();

    QSqlQuery query =DatabaseManager::instance().getPackageItemInfo(package);


    QJsonArray packageItemsArray;
    // 处理查询结果
    while (query.next())
    {
        QString itemName =query.value("item_name").toString();
        QString normalRange =query.value("normal_range").toString();

        QJsonObject itemObject;
        itemObject["itemName"] = itemName;
        itemObject["normalRange"] = normalRange;

         packageItemsArray.append(itemObject);
    }

    QJsonObject patientInfo;;
    patientInfo["packageItems"] =packageItemsArray;

    Packet packet =Protocol::createPacket(HealthCheckupItemResponce,patientInfo);

    QByteArray array =Protocol::serializePacket(packet);

    return array;
}


void ClientHandler::updateUserAppointments(const QString &selectdate)
{
    QJsonObject updateObj;
    updateObj["date"]=selectdate;

    Packet packet =Protocol::createPacket(UpdateAppointmentResponce,updateObj);
    QByteArray serializedResponse = Protocol::serializePacket(packet);

    emit requestProcessed(serializedResponse);

}




