#include "staffmainwindow.h"
#include "ui_staffmainwindow.h"

StaffMainWindow::StaffMainWindow(QWidget *parent,const QString &username) :
    QMainWindow(parent),m_username(username),
    ui(new Ui::StaffMainWindow)
{
    ui->setupUi(this);
}

StaffMainWindow::~StaffMainWindow()
{
    delete ui;
}
