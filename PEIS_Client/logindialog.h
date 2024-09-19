#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include<QDebug>
#include "registerdialog.h"
#include "clientsocket.h"
#include "passwordrecoverydialog.h"
#include "usermainwindow.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    int getUserRole()const;
    QString getUsername()const;

private slots:
    void onRegisterLabelClicked(const QString &link);               //处理注册标签点击事件
    void onForgetPasswordLabelClicked(const QString & link);        //处理找回密码标签点击事件
    void on_loginButton_clicked();

    void onLogined(const int &roleId,const QString & username);

private:
    Ui::LoginDialog *ui;
    int m_userRole;
    QString m_username;

};

#endif // LOGINDIALOG_H
