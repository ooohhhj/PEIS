#include <QApplication>
#include <QSqlQuery>
#include "serversocket.h"
#include "databasemanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 注册 QSqlQuery 为元类型
    qRegisterMetaType<QSqlQuery>("QSqlQuery");

    //连接客户端
    ServerSocket serverSocket;

    //连接数据库
    DatabaseManager::instance();

    return a.exec();
}
