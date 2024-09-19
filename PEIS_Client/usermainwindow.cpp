#include "usermainwindow.h"
#include "ui_usermainwindow.h"


UserMainWindow::UserMainWindow(QWidget *parent,const QString &username) :
    QMainWindow(parent),m_username(username),
    ui(new Ui::UserMainWindow)
{
    ui->setupUi(this);

    ui->usernameLabel->setText(this->m_username);
    qDebug()<<"username="<<this->m_username;




}

UserMainWindow::~UserMainWindow()
{
    delete ui;
}

void UserMainWindow::setUsername(const QString &username)
{
    this->m_username =username;
}
