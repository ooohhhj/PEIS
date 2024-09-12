#include "mainwindow.h"
#include "logindialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LoginDialog loginDialog;
    if(loginDialog.exec() == QDialog::Accepted)
    {
        //登录成功 创建并显示主窗口
        MainWindow w;
        w.show();
        return a.exec();
    }
    else
    {

        //登录失败 或用户关闭登录对话框，退出应用
        return 0;
    }
}
