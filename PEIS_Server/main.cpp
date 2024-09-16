#include <QCoreApplication>
#include "serversocket.h"
#include "databasemanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //连接客户端
    ServerSocket serverSocket;

    //连接数据库
    DatabaseManager::instance();

    return a.exec();
}
