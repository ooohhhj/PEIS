#include "clienthandler.h"


QMutex dbMutex;

ClientHandler::ClientHandler(QTcpSocket *socket, QObject *parent):clientSocket(socket)
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
        return handlePatientInfoRequest(message,PatientInfoResponce);
        break;
    case HealthCheckupItemRequest:
        return handleHealthCheckupItemRequest(message);
        break;
    case HealthCheckupDataRequest:
        return handleHealthCheckupDataRequest(message);
        break;
    case AppointmentInfoRequest:
        return handleAppointmentInfoRequest(message);
        break;
    case CancelAppointmentRequest:
        return handleCancelAppointmentRequest(message);
        break;
    case EditCheckupReportRequest:
        return handleEditCheckupReportRequest(message);
        break;
    case SaveReportRequest:
        return handleSaveReportRequest(message);
        break;
    case QueryHealthExaminationRecordsRequest:
        return handleQueryHealthExaminationRecordsRequest(message);
        break;
    case GetHealthExaminationRePortListRequest:
        return handleGetHealthExaminationRePortListRequest(message);
        break;
    case GetHealthExaminationRePortRequest:
        return handleGetHealthExaminationRePortRequest(message,GetHealthExaminationRePortResponce);
        break;
    case GetHealthExaminationRePortListRequest_Nurse:
        return handleGetHealthExaminationRePortRequest(message,GetHealthExaminationRePortResponce_Nurse);
        break;
    case GetHealthExaminationRePortListRequest_doctor:
        return handleGetHealthExaminationRePortRequest(message,GetHealthExaminationRePortResponce_doctor);
        break;
    case ElectronicMedicalRecordRequest:
        return handleElectronicMedicalRecordRequest(message);
        break;
    case PatientInfoRequest_Nurse:
        return handlePatientInfoRequest(message,PatientInfoResponce_Nurse);
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

            if(roleId == 2)
            {
                // 获取医生id
                int userId = DatabaseManager::instance().getUserIdByUsername(username);

                if (userId != -1)
                {
                    int doctorId = DatabaseManager::instance().getdoctorIdByDoctorTable(userId);
                    if (doctorId != -1)
                    {
                        InsertDoctorMapById(doctorId,clientSocket);
                    }
                }
            }

            if(roleId == 3)
            {
                // 获取用户id
                int userId = DatabaseManager::instance().getUserIdByUsername(username);

                if (userId != -1)
                {
                    InsertUserMapById(userId,clientSocket);
                }
            }


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

            responceMsg =StatusMessage::UsernameAndPasswordSuccessfully;


            if(roleId == 2)
            {
                // 获取医生id
                int userId = DatabaseManager::instance().getUserIdByUsername(username);

                if (userId != -1)
                {
                    int doctorId = DatabaseManager::instance().getdoctorIdByDoctorTable(userId);
                    if (doctorId != -1)
                    {
                        InsertDoctorMapById(doctorId,clientSocket);
                    }
                }
            }

            if(roleId == 3)
            {
                // 获取用户id
                int userId = DatabaseManager::instance().getUserIdByUsername(username);

                if (userId != -1)
                {
                    InsertUserMapById(userId,clientSocket);
                }
            }

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
    QMutexLocker locker(&dbMutex); // 加锁，确保在函数执行期间其他线程无法访问数据库
    QString username = checkupPackageDate["username"].toString();
    QString cardName = checkupPackageDate["cardName"].toString();
    QString selectDate = checkupPackageDate["selectDate"].toString();

    QJsonObject responseObject;
    QString message;

    // 获取用户和套餐ID
    int userId = DatabaseManager::instance().getUserIdByUsername(username);
    int packageId = DatabaseManager::instance().getPackageIdByName(cardName);

    // 检查获取的ID是否合法
    if (userId == -1 || packageId == -1) {
        message = StatusMessage::InternalServerError;
        responseObject["message"] = message;
        return Protocol::serializePacket(Protocol::createPacket(GetCheckupPackageCountResponce, responseObject));
    }


    // 检查用户是否已经预约
    if (DatabaseManager::instance().isUserAlreadyByAppointments(userId, packageId, selectDate)) {
        // 如果已预约，返回确认信息
        message = StatusMessage::UserAppointmentConfirmed;
    } else {
        // 如果未预约，检查是否还有可预约的名额
        int currentAppointmentCount = DatabaseManager::instance().getAppointmentCount(packageId, selectDate);
        int availablePackageCount = DatabaseManager::instance().getAvailablePackageCount(cardName, selectDate);

        if (currentAppointmentCount < availablePackageCount) {
            // 有空位，可以预约
            if (DatabaseManager::instance().insertAppointment(userId, packageId, selectDate)) {
                message = StatusMessage::AppointmentSuccessful;
                updateUserAppointments(selectDate); // 更新预约
            } else {
                message = StatusMessage::InternalServerError;
            }
        } else {
            // 无法预约，已满
            message = StatusMessage::CannotMakeanAppointment;
        }
    }

    // 构造响应
    responseObject["message"] = message;
    qDebug() << message;

    Packet packet = Protocol::createPacket(GetCheckupPackageCountResponce, responseObject);
    return Protocol::serializePacket(packet);
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

QByteArray ClientHandler::handlePatientInfoRequest(const QJsonObject &patienNameDate,const int &responceType)
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

    Packet packet =Protocol::createPacket(responceType,patientInfo);

    QByteArray array =Protocol::serializePacket(packet);

    return array;
}

QByteArray ClientHandler::handleHealthCheckupItemRequest(const QJsonObject &packageName)
{
    const QString package =packageName["packageName"].toString();

    const  QString patientName =packageName["patientName"].toString();

    const  QString appointmentDate = packageName["appointmentDate"].toString();

    //查询是否有这个记录

    bool ret =DatabaseManager::instance().isExistCheckupDate(package,patientName,appointmentDate);

    if(ret)
    {
        qDebug()<<"存在记录";//将结果发送

        QJsonObject obj;
        obj["Date"] =handleRecordHealthCheckup(patientName);

        Packet packet =Protocol::createPacket(RecordHealthCheckupResponce,obj);

        QByteArray array =Protocol::serializePacket(packet);
        return array;
    }
    else
    {
        qDebug()<<"不存在记录";
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
}

QByteArray ClientHandler::handleHealthCheckupDataRequest(QJsonObject &healthcheckupDate)
{
    //判断记录表是否存在记录
    QString patientName = healthcheckupDate["patientName"].toString();
    QString gender =healthcheckupDate["gender"].toString();
    QString birthDate =healthcheckupDate["birthDate"].toString();
    QString phoneNumber =healthcheckupDate["phoneNumber"].toString();
    QString packageName =healthcheckupDate["packageName"].toString();
    QString packageDate =healthcheckupDate["packageDate"].toString();

    QJsonArray HealthCheckupDate =healthcheckupDate["HealthCheckupDate"].toArray();
    QString status =healthcheckupDate["status"].toString();

    //判断记录表是否存在记录
    bool ret =DatabaseManager::instance().isExistCheckupDate(packageName,patientName,packageDate);

    if(!ret)
    {
        //不存在
        //插入
        ret = DatabaseManager::instance().InsertHealthCheckData(healthcheckupDate);
    }
    else
    {
        //更新
        ret = DatabaseManager::instance().UpdateHealthCheckData(healthcheckupDate);
    }

    if(status == "待审核")
    {
        //获取名字  修改预约表的状态
        int userId =DatabaseManager::instance().getUserIdByUsername(patientName);
        int packageId =DatabaseManager::instance().getPackageIdByName(packageName);


        ret = DatabaseManager::instance().updateStatusByAppointment(userId,packageId,packageDate);

        //发送给医生
        handlePendingUserData(userId,packageId,packageDate);
    }


    QJsonObject obj;
    QString message;
    if (ret)
    {
        message = StatusMessage::SubmissionSuccessful;
    }
    else
    {
        message = StatusMessage::SubmissionFailed;
    }

    obj["message"] = message;

    Packet packet = Protocol::createPacket(HealthCheckupItemDateResponce, obj);

    QByteArray array = Protocol::serializePacket(packet);

    return array;

}

QJsonArray ClientHandler::handleRecordHealthCheckup(const QString &patientName)
{
    QSqlQuery query =DatabaseManager::instance().getRecordHealthCheckupDate(patientName);

    QJsonArray DateArray;

    while(query.next())
    {
        QJsonObject dateObject;
        QString itemName = query.value("item_name").toString();
        QString normalRange = query.value("normal_range").toString();
        QString inputData = query.value("input_data").toString();
        QString  inputField= query.value("result_data").toString();
        QString responsiblePerson = query.value("responsible_person").toString();

        dateObject["item_name"] =itemName;
        dateObject["normal_range"] =normalRange;
        dateObject["input_data"] =inputData;
        dateObject["result_date"] =inputField;
        dateObject["responsible_person"] =responsiblePerson;

        DateArray.append(dateObject);
    }

    return DateArray;
}

QByteArray ClientHandler::handleAppointmentInfoRequest(const QJsonObject &usernameObj)
{
    QString username = usernameObj["username"].toString();


    QJsonObject obj;

    // 获取用户的预约信息
    QSqlQuery query = DatabaseManager::instance().getAppointmentInfoByusername(username);

    if (!query.next()) {
        // 没有找到预约信息
        obj["message"] = StatusMessage::GetAppointmentFalied;
        Packet packet = Protocol::createPacket(AppointmentInfoResponce, obj);
        QByteArray array = Protocol::serializePacket(packet);
        return array;  // 返回相应的对话框
    }

    // 获取预约状态及其他信息
    QString packageName = query.value("package_name").toString();
    QDateTime appointmentDate = query.value("appointment_date").toDateTime();
    QString appointmentStatus = query.value("status").toString();

    // 填充响应对象
    obj["username"] = username;
    obj["packageName"] = packageName;
    obj["appointmentDate"] = appointmentDate.toString();
    obj["status"] = appointmentStatus;

    obj["message"] = StatusMessage::GetAppointmentSuccessful;

    // 构造并序列化响应包
    Packet packet = Protocol::createPacket(AppointmentInfoResponce, obj);
    QByteArray array = Protocol::serializePacket(packet);

    return array;
}

QByteArray ClientHandler::handleCancelAppointmentRequest(const QJsonObject &AppointmentObj)
{
    QString username = AppointmentObj["username"].toString();
    QString packageName =AppointmentObj["packageName"].toString();
    QString appointmentDate =AppointmentObj["appointmentDate"].toString();


    bool ret =DatabaseManager::instance().CancelAppointment(username,packageName,appointmentDate);

    QJsonObject obj;
    QString message;
    if(ret)
    {
        message =StatusMessage::CancelAppointmentSuccessful;
        obj["appointmentDate"] =appointmentDate;
    }
    else
    {
        message =StatusMessage::InternalServerError;
    }
    obj["message"]=message;
    Packet packet =Protocol::createPacket(CancelAppointmentResponce,obj);
    QByteArray array = Protocol::serializePacket(packet);
    return array;
}

QByteArray ClientHandler::handleEditCheckupReportRequest(const QJsonObject &patientObj)
{
    QString patientName = patientObj["patientName"].toString();
    QString packageName =patientObj["packageName"].toString();
    QString appointmentDate =patientObj["appointmentDate"].toString();

    qDebug()<<"patientName="<<patientName;
    qDebug()<<"packageName="<<packageName;
    qDebug()<<"appointmentDate="<<appointmentDate;

    //查询是否有这个记录
    QSqlQuery query = DatabaseManager::instance().getPatientCheckupDate(patientName,packageName,appointmentDate);

    //获取病人信息

    if(!query.next())
    {
        return QByteArray();
    }

    patientName = query.value("patient_name").toString();
    QString gender = query.value("gender").toString();
    QString birth_date = query.value("birth_date").toString();
    QString phone_number = query.value("phone_number").toString();
    QString package_name = query.value("package_name").toString();
    QString package_date = query.value("package_date").toString();
    QString report_generated =query.value("report_generated").toString();
    QString report_status =query.value("report_status").toString();
    QString doctor_name =query.value("doctor_name").toString();
    QString doctor_advice =query.value("doctor_advice").toString();

    QJsonObject obj;

    obj["patientName"]=patientName;
    obj["gender"]=gender;
    obj["birth_date"]=birth_date;
    obj["phone_number"]=phone_number;
    obj["package_name"]=package_name;
    obj["package_date"]=package_date;
    obj["report_generated"]=report_generated;
    obj["report_status"]=report_status;
    obj["doctor_name"]=doctor_name;
    obj["doctor_advice"]=doctor_advice;


    obj["CheckupDate"] =handleRecordHealthCheckup(patientName);

    Packet packet =Protocol::createPacket(PatientHealthExaminationDateResponce,obj);

    QByteArray array =Protocol::serializePacket(packet);

    return array;

}

QByteArray ClientHandler::handleEditCheckupReportRequest2(const QJsonObject &doctornameObj)
{
    QString doctorName = doctornameObj["doctorname"].toString();

    QJsonObject patientInfo;
    // 获取 doctorId
    int userId = DatabaseManager::instance().getUserIdByUsername(doctorName);
    if (userId == -1) {
        // 返回错误信息
        patientInfo["message"] = "Invalid doctor username.";
        return Protocol::serializePacket(Protocol::createPacket(InternalServerError, patientInfo));
    }

    int doctorId = DatabaseManager::instance().getdoctorIdByDoctorTable(userId);
    if (doctorId == -1) {
        // 返回错误信息
        patientInfo["message"] = "Doctor ID not found.";
        return Protocol::serializePacket(Protocol::createPacket(InternalServerError, patientInfo));
    }

    // 获取健康检查患者信息
    QList<QVariantMap> checkupList = DatabaseManager::instance().GetHealthExaminationPatientInfo(doctorId);

    QJsonArray checkupArray;

    // 遍历每个健康检查信息
    for (const QVariantMap &checkup : checkupList) {
        QJsonObject checkupObject;

        // 填充健康检查信息
        checkupObject["health_checkup_id"] = checkup["health_checkup_id"].toInt();
        checkupObject["patient_name"] = checkup["patient_name"].toString();
        checkupObject["gender"] = checkup["gender"].toString();
        checkupObject["birth_date"] = checkup["birth_date"].toString();
        checkupObject["phone_number"] = checkup["phone_number"].toString();
        checkupObject["package_name"] = checkup["package_name"].toString();
        checkupObject["report_generated"] = checkup["report_generated"].toString();
        checkupObject["report_status"] = checkup["report_status"].toString();
        checkupObject["appointment_date"] = checkup["appointment_date"].toString();

        // 打印健康检查信息
        qDebug() << "Health Checkup Info: " << checkupObject;

        // 使用 health_checkup_id 查询健康检查项目
        int healthCheckupId = checkup["health_checkup_id"].toInt();
        QList<QVariantMap> checkupItems = DatabaseManager::instance().GetPatientHealthExaminationData(healthCheckupId);

        // 创建项目数组
        QJsonArray itemsArray;

        // 遍历每个健康检查项目
        for (const QVariantMap &item : checkupItems) {
            QJsonObject itemObject;
            itemObject["health_checkup_item_id"] = item["health_checkup_item_id"].toInt();
            itemObject["health_checkup_id"] = item["health_checkup_id"].toInt();
            itemObject["item_name"] = item["item_name"].toString();
            itemObject["normal_range"] = item["normal_range"].toString();
            itemObject["input_data"] = item["input_data"].toString();
            itemObject["result_data"] = item["result_data"].toString();
            itemObject["responsible_person"] = item["responsible_person"].toString();

            // 添加项目对象到数组
            itemsArray.append(itemObject);

            // 打印健康检查项目的信息
            qDebug() << "Health Checkup Item: " << itemObject;
        }

        // 将项目数组添加到健康检查对象
        checkupObject["items"] = itemsArray;

        // 将健康检查对象添加到主数组
        checkupArray.append(checkupObject);
    }

    patientInfo["patientArray"] = checkupArray;

    // 创建并序列化数据包
    Packet packet = Protocol::createPacket(PatientHealthExaminationDateResponce, patientInfo);
    return Protocol::serializePacket(packet);

}

QByteArray ClientHandler::handleSaveReportRequest(const QJsonObject &reportDateObj)
{
    QString patientName = reportDateObj["patientName"].toString();
    QString packageName = reportDateObj["packageName"].toString();
    QString packageDate =reportDateObj["packageDate"].toString();


    qDebug()<<"patientName="<<patientName;
    qDebug()<<"packageName="<<packageName;
    qDebug()<<"packageDate="<<packageDate;


    //将信息填入数据库
    bool ret =DatabaseManager::instance().updatePatientReport(reportDateObj);

    QString message;
    if(!ret)
    {
        qDebug()<<"更新失败";
        message=StatusMessage::InternalServerError;
        qDebug() << "未找到病人信息，请检查数据库查询参数。";
    }
    else
    {
        //生成报告
        //获取数据
        QSqlQuery query = DatabaseManager::instance().getPatientCheckupDate(patientName,packageName,packageDate);

        //获取病人信息
        if(!query.next())
        {
            message=StatusMessage::InternalServerError;
            qDebug()<<"执行";
        }
        else
        {
            QJsonArray dateArray = handleRecordHealthCheckup(patientName);


            emit generateReport(clientSocket,query, dateArray, patientName, packageName, packageDate);

        }
    }


    if(message.isEmpty())
    {
        return QByteArray();
    }

    QJsonObject obj;

    obj["message"]=message;


    Packet packet =Protocol::createPacket(SaveReportResponce,obj);

    QByteArray array = Protocol::serializePacket(packet);

    return array;

}

QByteArray ClientHandler::handleQueryHealthExaminationRecordsRequest(const QJsonObject &userNameDate)
{
    QString username = userNameDate["username"].toString();

    QSqlQuery query =DatabaseManager::instance().getAllAppointmentInfoByusername(username);

    // 如果查询失败，返回错误信息
    if (!query.isActive() || !query.isSelect()) {
        QJsonObject errorResponse;
        errorResponse["status"] = "error";
        errorResponse["message"] = "Failed to retrieve appointment information.";
        QJsonDocument errorDoc(errorResponse);
        return QByteArray();
    }

    // 准备一个 JSON 数组存储所有预约信息
    QJsonArray RecordsArray;

    // 遍历查询结果
    while (query.next()) {
        QJsonObject appointment;

        // 提取每个预约的字段信息
        QString healthPackage = query.value("package_name").toString();
        QDate appointmentDate = query.value("appointment_date").toDate();
        QString appointmentStatus = query.value("status").toString();

        // 将每条预约信息存入 JSON 对象
        appointment["health_package"] = healthPackage;
        appointment["appointment_date"] = appointmentDate.toString(Qt::ISODate);
        appointment["appointment_status"] = appointmentStatus;

        // 将该预约信息添加到 JSON 数组中
        RecordsArray.append(appointment);
    }

    QJsonObject Info;
    Info["records"] =RecordsArray;

    Packet packet =Protocol::createPacket(QueryHealthExaminationRecordsResponce,Info);

    QByteArray array =Protocol::serializePacket(packet);

    return array;

}

QByteArray ClientHandler::handleGetHealthExaminationRePortListRequest(const QJsonObject &userNameDate)
{
    QString username = userNameDate["username"].toString();

    QSqlQuery query =DatabaseManager::instance().getAppointmentFinshInfoByusername(username);

    // 如果查询失败，返回错误信息
    if (!query.isActive() || !query.isSelect()) {
        QJsonObject errorResponse;
        errorResponse["status"] = "error";
        errorResponse["message"] = "Failed to retrieve appointment information.";
        QJsonDocument errorDoc(errorResponse);
        return QByteArray();
    }

    // 准备一个 JSON 数组存储所有预约信息
    QJsonArray RecordsArray;

    // 遍历查询结果
    while (query.next()) {
        QJsonObject appointment;

        // 提取每个预约的字段信息
        QString healthPackage = query.value("package_name").toString();
        QDate appointmentDate = query.value("appointment_date").toDate();
        QString appointmentStatus = query.value("status").toString();

        qDebug()<<"healthPackage="<<healthPackage;
        qDebug()<<"appointmentDate="<<appointmentDate;
        qDebug()<<"appointmentStatus="<<appointmentStatus;


        // 将每条预约信息存入 JSON 对象
        appointment["health_package"] = healthPackage;
        appointment["appointment_date"] = appointmentDate.toString(Qt::ISODate);
        appointment["appointment_status"] = appointmentStatus;

        // 将该预约信息添加到 JSON 数组中
        RecordsArray.append(appointment);
    }

    QJsonObject Info;
    Info["reportsList"] =RecordsArray;

    Packet packet =Protocol::createPacket(GetHealthExaminationRePortListResponce,Info);

    QByteArray array =Protocol::serializePacket(packet);

    return array;

}

QByteArray ClientHandler::handleGetHealthExaminationRePortRequest(const QJsonObject &reportRequestDate, const int &responceType)
{
    QString username =reportRequestDate["patientName"].toString();
    QString packageName =reportRequestDate["packageName"].toString();
    QString appointmentDate =reportRequestDate["appointmentDate"].toString();

    QString reportPath = DatabaseManager::instance().getUserReport(username,packageName,appointmentDate);

    QString message;
    QByteArray pdfData;

    if(reportPath.isEmpty())
    {
        message = StatusMessage::InternalServerError;
    }
    else
    {
        // 获取体检报告 PDF
        QFile pdfFile(reportPath);
        if (!pdfFile.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open PDF file:" << pdfFile.errorString();
            message = StatusMessage::InternalServerError;
        } else {
            pdfData = pdfFile.readAll();
            pdfFile.close();
        }
    }

    QJsonObject obj;
    Packet packet;
    QByteArray array;
    if(!message.isEmpty() || pdfData.isEmpty() )
    {
        obj["message"]=message;
    }
    else
    {
        QStringList pathParts = reportPath.split("/");

        // 提取姓名（假设姓名为倒数第二部分）
        QString name = pathParts[pathParts.size() - 2];

        // 提取体检类型（假设体检类型为倒数第一部分前的部分）
        QString examinationType = pathParts[pathParts.size() - 1];
        examinationType = examinationType.split("_").at(0);  // 取下划线前的部分

        // 提取日期（假设日期为文件名的前8位，前提是文件名为日期+下划线+其他内容）
        QString fileName = pathParts[pathParts.size() - 1];  // 获取文件名部分
        QString date = fileName.mid(0, 8);  // 获取前8位作为日期

        // 创建新的文件名称
        QString newFileName = QString("%1_%2_%3.pdf").arg(name, examinationType, date);

        obj["fileName"]=newFileName;
        obj["fileType"] = "application/pdf";  // 文件类型
        obj["fileData"]=QString(pdfData.toBase64());
        // 创建数据包，指定通信类型为 SendPDF
    }
    packet = Protocol::createPacket(responceType,obj);

    array =Protocol::serializePacket(packet);

    return array;
}

QByteArray ClientHandler::handleElectronicMedicalRecordRequest(const QJsonObject &userNameDate)
{
    QString username = userNameDate["username"].toString();

    QSqlQuery query =DatabaseManager::instance().getAppointmentInfoFinshByusername(username);

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

    Packet packet =Protocol::createPacket(ElectronicMedicalRecordResponce,appointmentInfo);

    QByteArray array =Protocol::serializePacket(packet);

    return array;
}

void ClientHandler::handlePendingUserData(const int &patientId, const int &packageId, const QString &appointmentDate)
{

    QString formattedDate = QDate::fromString(appointmentDate, "yyyy-M-d").toString("yyyy-MM-dd");

    int doctorId =DatabaseManager::instance().getDoctorIdByAppointments(patientId,packageId,formattedDate);


    // 检查医生 ID 是否有效
    if (doctorId <= 0) {
        qDebug() << "无效的医生 ID:" << doctorId;
        emit requestProcessed(QByteArray()); // 返回空的 QByteArray 或适当处理
        return ;
    }


    emit ForwardDoctorMessage(doctorId);

}

void ClientHandler::updateUserAppointments(const QString &selectdate)
{
    QJsonObject updateObj;
    updateObj["date"]=selectdate;

    Packet packet =Protocol::createPacket(UpdateAppointmentResponce,updateObj);
    QByteArray serializedResponse = Protocol::serializePacket(packet);

    emit requestProcessed(serializedResponse);

}




