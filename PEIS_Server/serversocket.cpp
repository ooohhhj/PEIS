#include "serversocket.h"

ServerSocket::ServerSocket(QObject *parent) : QObject(parent)
{
     tcpServer = new QTcpServer(this);
     threadPool = new QThreadPool(this);

     //设置线程池中最大线程数
     threadPool->setMaxThreadCount(100);

     //启动服务端
     if(!tcpServer->listen(QHostAddress::Any,9600))
     {
         qDebug()<<"Server could not start!";
     }
     else
     {
         qDebug()<<"Server started";
         connect(tcpServer,&QTcpServer::newConnection,this,&ServerSocket::newConnection);
     }
}

void ServerSocket::newConnection()
{
    qDebug()<<"有新的客户端";
    while(tcpServer->hasPendingConnections())
    {
        QTcpSocket *clientSocket = tcpServer->nextPendingConnection();

        //当有数据可读时处理
        connect(clientSocket,&QTcpSocket::readyRead,this,[this,clientSocket]()
        {
           //创建工作对象
           ClientHandler * handler =new ClientHandler(clientSocket);

           //接收线程处理后的数据 发送给客户端
           connect(handler,&ClientHandler::requestProcessed,this,[this,clientSocket](const QByteArray & response)
           {
               //将处理结果发送给客户端
               clientSocket->write(response);
           });

            //将数据交给线程池中的任务处理
           threadPool->start(handler);
        });

        //监听客户端断开连接
        connect(clientSocket,&QTcpSocket::disconnected,clientSocket,&QTcpSocket::deleteLater);

    }
}
