#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QThreadPool>
#include "clienthandler.h"


class ServerSocket : public QObject
{
    Q_OBJECT
public:
    explicit ServerSocket(QObject *parent = nullptr);

private slots:
    void newConnection();

private:
    QTcpServer * tcpServer;     //Tcp服务器对象
    QThreadPool *threadPool;    //线程池对象

signals:

};

#endif // SERVERSOCKET_H
