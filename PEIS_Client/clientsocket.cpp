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
    QJsonObject registerMessage = Protocol::parsePacket(packet);

    // 如果要检查长度，可以对 JSON 对象序列化后进行字节长度检查
    QByteArray jsonData = QJsonDocument(registerMessage).toJson(QJsonDocument::Compact);

    if(packet.length!=jsonData.size())
    {
        qDebug() << "数据包长度与实际消息内容不匹配: packet.length = "
                 << packet.length << ", message.size() = " << registerMessage.size();
    }

    switch (packet.requestType)
    {
    case UsernameIsExistResponce:
    {
        QString message =registerMessage["message"].toString();
        emit usernameIsExist(message);
        break;
    }
    case UsernameNotExistResponce:
    {
        //用户名不存在 不做处理
        break;
    }
    case RegisterResponce:
    {
        QString message =registerMessage["message"].toString();
        QMessageBox* msgBox = new QMessageBox(QMessageBox::Information,"注册账号",message);
        msgBox->setStandardButtons(QMessageBox::NoButton);//不显示按钮

        //创建一个定时器 2秒后关闭消息框
        QTimer::singleShot(2000,msgBox,&QMessageBox::accept);
        msgBox->show();

        emit RegisterSuccessfuly();
        break;
    }
    case InternalServerError:
    {
        QString message =registerMessage["message"].toString();
        QMessageBox* msgBox = new QMessageBox(QMessageBox::Warning,"警告",message);
        msgBox->setStandardButtons(QMessageBox::NoButton);//不显示按钮
        //创建一个定时器 2秒后关闭消息框
        QTimer::singleShot(2000,msgBox,&QMessageBox::accept);

        msgBox->show();
        break;
    }
    default:
        qDebug()<<"未知的请求类型:"<<packet.requestType;
    }
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


