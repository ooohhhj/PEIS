#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <QMainWindow>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QThreadPool>
#include <QMap>
#include <QReadWriteLock>
#include <QTextDocument>
#include <QJsonObject>
#include <QDir>

#include "clienthandler.h"

namespace Ui {
class ServerSocket;
}

class ServerSocket : public QMainWindow
{
    Q_OBJECT

public:
    explicit ServerSocket(QWidget *parent = nullptr);
    ~ServerSocket();

    QString generateHealthCheckupReport(const QSqlQuery &query, const QJsonArray &dateArray);

     void UserForwardMessage(const int & userId);

private slots:
    void newConnection();
    void InsertDoctorMapById(const int & doctorId, QTcpSocket*clientSocket);
    void InsertUserMapById(const int & userId, QTcpSocket*clientSocket);
    void ForwardDoctorMessage(const int & doctorId);
    void onGenerateReport(QTcpSocket *socket,const QSqlQuery &query, const QJsonArray &dateArray, const QString &patientName, const QString &packageName, const QString &packageDate);


private:
    QTcpServer * tcpServer;     //Tcp服务器对象
    QThreadPool *threadPool;    //线程池对象
    QMap<int, QTcpSocket*> doctorConnections;  // 用于存储医生 ID 和相应的网络连接
    QMap<int,QTcpSocket*> UserConnections;      //用户存储用户ID和相应的网络连接
    QReadWriteLock rwLock; // 读写锁
    Ui::ServerSocket *ui;
};

#endif // SERVERSOCKET_H
