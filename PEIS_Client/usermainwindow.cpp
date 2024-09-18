#include "usermainwindow.h"
#include "ui_usermainwindow.h"


UserMainWindow::UserMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UserMainWindow)
{
    ui->setupUi(this);

    qDebug()<<"显示";
}

UserMainWindow::~UserMainWindow()
{
    delete ui;
}
