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

    QString message =MessageObject["message"].toString();

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
    case InternalServerError:
    {
        showMessageBox(":/warning.png","警告",message);
        break;
    }
    default:
        showMessageBox(":/warning.png","警告","未知的请求类型");
        break;
    }
}

void ClientSocket::showMessageBox(const QString &iconPath, const QString &windowsTitle, const QString &message)
{

    QMessageBox msgBox;
    msgBox.setWindowTitle(windowsTitle);
    msgBox.setText(message);

    // 设置字体
    QFont font;
    font.setPointSize(14);  // 设置字体大小，例如 14pt
    font.setFamily("Microsoft YaHei");  // 设置支持中文的字体类型
    msgBox.setFont(font);


    //使用图标路径加载QPixmap 并设置图标
    QPixmap pixmap(iconPath);
    if(!pixmap.isNull())
    {
        // 将图标缩放到 50x50
        QPixmap scaledPixmap = pixmap.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        msgBox.setIconPixmap(scaledPixmap);
    }
    else
    {
        qDebug()<<"图标加载失败"<<endl;
    }

    // // 设置文本颜色为黑色
    msgBox.setStyleSheet("QLabel { color: black;  }"  );
    //创建一个定时器 5秒后关闭消息框
    QTimer::singleShot(2000,&msgBox,&QMessageBox::accept);

    msgBox.setStandardButtons(QMessageBox::NoButton);


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


