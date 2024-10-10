#include "clientsocket.h"

ClientSocket::ClientSocket(QObject *parent) : QObject(parent)
{
    tcpSocket =new QTcpSocket(this);

}

ClientSocket *ClientSocket::instance()
{
    static ClientSocket instance;
    return & instance;
}

void ClientSocket::connectToServer(const QString & host,quint16 port)
{
    if(!tcpSocket)
    {
        qDebug()<<"tcpSocket is null"<<endl;
        return;
    }

    //连接服务端
    tcpSocket->connectToHost(host,port);

    connect(tcpSocket,&QTcpSocket::connected,this,[this]()
    {
        qDebug()<<"Connected to server!";
    });

    connect(tcpSocket,&QTcpSocket::readyRead,this,&ClientSocket::readData);

}

void ClientSocket::senData(const QByteArray &data)
{
    if(tcpSocket->state()==QAbstractSocket::ConnectedState)
    {
        qDebug()<<"发送的数据="<<data;
        qint64 bytesWritten = tcpSocket->write(data);
        if (bytesWritten == -1) {
            qDebug() << "Failed to write data:" << tcpSocket->errorString();
        } else {
            tcpSocket->flush(); // 确保数据被发送
            qDebug() << "Data sent successfully!";
        }
    }
    else
    {
        qDebug()<<"Socket is not connected";
    }
}

void ClientSocket::processResponse(Packet &packet)
{
    //检查解包后的消息长度是否与包的长度匹配
    QJsonObject MessageObject = Protocol::parsePacket(packet);

    // 如果要检查长度，可以对 JSON 对象序列化后进行字节长度检查
    QByteArray jsonData = QJsonDocument(MessageObject).toJson(QJsonDocument::Compact);

    if(packet.length!=jsonData.size())
    {
        qDebug() << "数据包长度与实际消息内容不匹配: packet.length = "
                 << packet.length << ", message.size() = " << MessageObject.size();
    }

    QString message;
    if (MessageObject.contains("message")) {
        message = MessageObject["message"].toString();
        // 处理 message
    }


    switch (packet.requestType)
    {
    case UsernameIsExistResponce:
    {
        emit usernameIsExist(message);
        break;
    }
    case UsernameNotExistResponce:
    {
        //用户名不存在 不做处理
        break;
    }
    case RegisterSuccessfullyResponce:
    {
        showMessageBox(":/successfully.png","注册账号",message);
        emit RegisterSuccessfully();
        break;
    }
    case RegisterFailedResponce:
    {
        showMessageBox(":/failed.png","注册账号",message);
        break;
    }
    case PhoneNumberIsExistResponce:
    {
        //手机号存在  不做任何处理
        break;
    }
    case PhoneNumberNotExistResponce:
    {
        emit PhoneNumberNotExist(message);
        break;
    }
    case ForgetPasswordSusseccfullyResponce:
    {
        showMessageBox(":/successfully.png","找回密码",message);
        emit ForgetPasswordSuccessfully();
        break;
    }
    case ForgetPasswordFailedResponce:
    {
        showMessageBox(":/failed.png","找回密码",message);
        break;
    }
    case PhoneAndPasswordSuccessfullyResponce:
    {
        int roleId =MessageObject["roleId"].toInt();
        QString username =MessageObject["username"].toString();
        emit Logined(roleId,username);
        break;
    }
    case PhoneAndPasswordFailedResponce:
    {
        showMessageBox(":/failed.png","登录",message);
        break;
    }
    case UsernameAndPasswordSuccessfullyResponce:
    {
        int roleId =MessageObject["roleId"].toInt();
        QString username =MessageObject["username"].toString();
        emit Logined(roleId,username);
        break;
    }
    case USernameAndPasswordFailedResponce:
    {
        showMessageBox(":/failed.png","登录",message);
        break;
    }
    case ReserveCheckupResponce:
    {
        //获取套餐消息
        QJsonArray packagesArray = MessageObject["packages"].toArray();
        int totalPages = MessageObject["totalPages"].toInt();
        emit ReserveCheckup(packagesArray,totalPages);

        break;
    }
    case PackageInformationResponce:
    {
        QJsonArray packageInfoArray = MessageObject["packageInfo"].toArray();
        QString packageDescription =MessageObject["package_description"].toString();
        emit PackageNameInfo(packageInfoArray,packageDescription);
        break;
    }
    case SearchPackageResponce:
    {
        QJsonArray packagesArray = MessageObject["packages"].toArray();
        int totalPages = MessageObject["totalPages"].toInt();
        emit ReserveCheckup(packagesArray,totalPages);
        break;
    }
    case GetCheckupPackageCountResponce:
    {

        if(message == StatusMessage::AppointmentSuccessful)
        {
            showMessageBox(":/successfully.png","预约套餐",message);
        }
        else
        {
            showMessageBox(":/warning.png","预约套餐",message);
        }
        break;
    }
    case UpdateAppointmentResponce:
    {
        QString selectDate =MessageObject["date"].toString();
        emit updateUserAppointment(selectDate,"已预约");
        break;
    }
    case UserInfoResponce:
    {
        QJsonArray array =MessageObject["userInfos"].toArray();
        emit userInfos(array);
        break;
    }
    case StartDateResponce:
    {
        QString startDate =MessageObject["startDate"].toString();
        emit OnstartDate(startDate);
        break;
    }
    case AppointmentInformationResponce:
    {
        QJsonArray array =MessageObject["appointments"].toArray();
        emit OnAppointmentsDate(array);
        break;
    }
    case PatientInfoResponce:
    {
        QJsonArray patientInfo =MessageObject["patientInfo"].toArray();
        emit OnPatientInfoResponce(patientInfo);
        break;
    }
    case InternalServerError:
    {
        showMessageBox(":/warning.png","警告",message);
        break;
    }
    case HealthCheckupItemResponce:
    {
        QJsonArray packageItems =MessageObject["packageItems"].toArray();
        emit OnHealthCheckupItemResponce(packageItems);
        break;
    }
    case  HealthCheckupItemDateResponce:
    {
        QString message =MessageObject["message"].toString();
        if(message ==StatusMessage::SubmissionSuccessful)
        {
            showMessageBox(":/successfully.png","患者信息",message);
        }
        else
        {
            showMessageBox(":/warning.png","患者信息",message);
        }
        break;
    }
    case RecordHealthCheckupResponce:
    {
        QJsonArray date =MessageObject["Date"].toArray();
        emit OnRecordHealthCheckupResponce(date);
        break;
    }
    case AppointmentInfoResponce:
    {
        QString message =MessageObject["message"].toString();

        if(message == StatusMessage::GetAppointmentSuccessful)
        {
            QString username =MessageObject["username"].toString();
            QString packageName =MessageObject["packageName"].toString();
            QString appointmentDate =convertAppointmentDate(MessageObject["appointmentDate"].toString());
            QString status =MessageObject["status"].toString();

            emit onGetAppointmentInfo(username,packageName,appointmentDate,status);
        }
        else if(message == StatusMessage::GetAppointmentFalied)
        {
            qDebug()<<"message"<<message;
            showMessageBox(":/warning.png","预约信息",message);
        }
        break;
    }
    case CancelAppointmentResponce:
    {
        QString message =MessageObject["message"].toString();
        QString appointmentDate =MessageObject["appointmentDate"].toString();

        if(message ==StatusMessage::CancelAppointmentSuccessful)
        {
            showMessageBox(":/successfully.png","预约信息",message);
            QString selectDate =MessageObject["appointmentDate"].toString();
            qDebug()<<"selectDate="<<selectDate;
            emit updateUserAppointment(selectDate,"预约");
        }
        else
        {
            showMessageBox(":/warning.png","预约信息",message);
        }

        break;
    }
    case PatientHealthExaminationReview:
    {
        emit OnPatientHealthExaminationReview();
        break;
    }
    case PatientHealthExaminationDateResponce:
    {
        emit OnPatientHealthExaminationDateResponce(MessageObject);
        break;
    }
    case SaveReportResponce:
    {
        if(message == StatusMessage::GenerateReportSuccessfully)
        {
            showMessageBox(":/successfully.png","体检报告",message);
        }
        else if(message == StatusMessage::WaitGenerateReport)
        {
            showMessageBox(":/warning.png","体检报告",message);
        }
        else
        {
            showMessageBox(":/warning.png","体检报告",message);
        }
        break;
    }
    case UserCheckupGenerateNotice:
    {
        emit OnUserCheckupGenerateNotice();
        break;
    }
    case QueryHealthExaminationRecordsResponce:
    {
        QJsonArray array =MessageObject["records"].toArray();
        emit OnHealthExaminationRecordsResponce(array);
        break;
    }
    default:
        showMessageBox(":/warning.png","警告","未知的请求类型");
        break;
    }
}

void ClientSocket::showMessageBox(const QString &iconPath, const QString &windowsTitle, const QString &message)
{

    QDialog  msgBox;
    msgBox.setWindowTitle(windowsTitle);
    msgBox.setMinimumSize(400, 200); // 设置最小尺寸

    // 去掉帮助按钮 (?)
    msgBox.setWindowFlags(msgBox.windowFlags() & ~Qt::WindowContextHelpButtonHint);


    // 设置字体
    QFont font;
    font.setPointSize(14);  // 设置字体大小，例如 14pt
    font.setFamily("Microsoft YaHei");  // 设置支持中文的字体类型

    QHBoxLayout *layout = new QHBoxLayout(&msgBox);
    QLabel *label = new QLabel(message);
    label->setFont(font);
    label->setStyleSheet("QLabel { color: black; }");
    layout->addWidget(label);  // 添加文本标签


    QPixmap pixmap(iconPath);
    if (!pixmap.isNull()) {
        QPixmap scaledPixmap = pixmap.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QLabel *iconLabel = new QLabel;
        iconLabel->setPixmap(scaledPixmap);
        layout->addWidget(iconLabel);
    }

    layout->addWidget(label);
    msgBox.setLayout(layout);

    // 设置定时器，2秒后关闭对话框
    QTimer::singleShot(2000, &msgBox, &QDialog::accept);

    //创建一个定时器 5秒后关闭消息框
    QTimer::singleShot(2000,&msgBox,&QMessageBox::accept);


    msgBox.exec();
}

void ClientSocket::readData()
{
    QByteArray data =tcpSocket->readAll();
    qDebug()<<"Received data from server:"<<data;

    //解包
    Packet packet = Protocol::deserializePacket(data);

    //根据类型处理数据
    processResponse(packet);
}

QString ClientSocket::convertAppointmentDate(const QString &inputDate)
{
    // 尝试解析不同格式的日期
    QDateTime dateTime;

    // 尝试解析"周日 9月 29 00:00:00 2024"格式
    dateTime = QDateTime::fromString(inputDate, "ddd M月 d HH:mm:ss yyyy");
    if (dateTime.isValid()) {
        return dateTime.toString("yyyy-M-d");
    }

    // 尝试解析"周三 6月 5 2002"格式
    dateTime = QDateTime::fromString(inputDate, "ddd M月 d yyyy");
    if (dateTime.isValid()) {
        return dateTime.toString("yyyy-M-d");
    }

    // 尝试解析"2024-09-29 00:00:00"格式
    dateTime = QDateTime::fromString(inputDate, "yyyy-MM-dd HH:mm:ss");
    if (dateTime.isValid()) {
        return dateTime.toString("yyyy-M-d");
    }

    // 如果没有匹配的格式，返回一个默认值或错误提示
    qDebug() << "日期解析失败: " << inputDate;
    return QString();
}


