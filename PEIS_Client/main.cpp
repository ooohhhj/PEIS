#include "adminmainwindow.h"
#include "staffmainwindow.h"
#include "usermainwindow.h"
#include "logindialog.h"

#include <QApplication>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    //    StaffMainWindow staffWindow;
    //    staffWindow.show();
    //    return a.exec();

    LoginDialog loginDialog;

    if(loginDialog.exec() == QDialog::Accepted)
    {
        int userRole =loginDialog.getUserRole();
        QString username =loginDialog.getUsername();
        qDebug()<<"username="<<username;

        switch (userRole)
        {
        case 1:
        {
            AdminMainWindow *adminWindow =new AdminMainWindow(nullptr,username);
            adminWindow->show();
        }
            break;
        case 2:
        {
            StaffMainWindow *staffWindow =new StaffMainWindow(nullptr,username);
            staffWindow->show();
        }
            break;
        case 3:
        {

            qDebug() << "User role: 3";
            UserMainWindow *userWindow = new UserMainWindow(nullptr,username);
            userWindow->show();
            break;
        }
        default:
            QMessageBox::warning(nullptr, "Error", "Unknown user role.");
            return 0;
            break;
        }


        return a.exec();
    }
    else
    {
        //登录失败 或用户关闭登录对话框，退出应用
        return 0;
    }
}
