#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QThreadPool>
#include <QMap>
#include <QReadWriteLock>


#include "clienthandler.h"


class ServerSocket : public QObject
{
    Q_OBJECT
public:
    explicit ServerSocket(QObject *parent = nullptr);


private slots:
    void newConnection();
    void InsertDoctorMapById(const int & doctorId, QTcpSocket*clientSocket);
    void ForwardMessage(const int & doctorId);

private:
    QTcpServer * tcpServer;     //Tcp服务器对象
    QThreadPool *threadPool;    //线程池对象
    QMap<int, QTcpSocket*> doctorConnections;  // 用于存储医生 ID 和相应的网络连接
    QReadWriteLock rwLock; // 读写锁
signals:

};

#endif // SERVERSOCKET_H
