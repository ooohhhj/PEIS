#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QIcon>
#include <QGridLayout>
#include <QLabel>
#include <QJsonArray>

#include "protocol.h"

class ClientSocket : public QObject
{
    Q_OBJECT
public:
    explicit ClientSocket(QObject *parent = nullptr);

    //使用单例模式 获取ClientSocket实例
    static ClientSocket* instance();


    //禁止复制和赋值操作
    ClientSocket(const ClientSocket&) =delete;
    ClientSocket& operator=(const ClientSocket&) =delete;

    void connectToServer(const QString & host,quint16 port);

    void senData(const QByteArray&data);

    void processResponse(Packet &packet);

    void showMessageBox(const QString &iconPath, const QString &windowsTitle,const QString &message);




private slots:
    void readData();
signals:
    void usernameIsExist(const QString&message);
    void RegisterSuccessfully();
    void ForgetPasswordSuccessfully();
    void PhoneNumberNotExist(const QString & message);

    void Logined(const int &roleId,const QString &username);

    void ReserveCheckup(const QJsonArray & jsonArray,const int& totalPage);
private:
    QTcpSocket * tcpSocket;

};

#endif // CLIENTSOCKET_H
