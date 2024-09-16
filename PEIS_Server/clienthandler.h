#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QObject>
#include<QTcpSocket>
#include<QDebug>
#include<QRunnable>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlQuery>
#include <QJsonArray>


#include "protocol.h"
#include "databasemanager.h"

class ClientHandler : public QObject,public QRunnable
{
    Q_OBJECT
public:
    explicit ClientHandler(QTcpSocket*socket,QObject *parent = nullptr);

    virtual void run() override;

signals:
    // 当处理完请求后，可能会发出一些信号给服务端
       void requestProcessed(const  QByteArray &response);
private:
    QByteArray processRequest(Packet &packet);

    QByteArray handleRegisterRequest(const QJsonObject& registerDate);

    QByteArray handleUsernameIsRequest(const QJsonObject& usernameDate);


private:
    QTcpSocket * clientSocket; //客户端的套接字
    QByteArray data;           // 用于存储读取的客户端数据
};

#endif // CLIENTHANDLER_H
