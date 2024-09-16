#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include<QDebug>
#include "registerdialog.h"
#include "clientsocket.h"
#include "passwordrecoverydialog.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void onRegisterLabelClicked(const QString &link);               //处理注册标签点击事件
    void onForgetPasswordLabelClicked(const QString & link);        //处理找回密码标签点击事件
private:
    Ui::LoginDialog *ui;

};

#endif // LOGINDIALOG_H
