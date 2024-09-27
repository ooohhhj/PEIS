#include "staffmainwindow.h"
#include "ui_staffmainwindow.h"

StaffMainWindow::StaffMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StaffMainWindow)
{
    ui->setupUi(this);
}

StaffMainWindow::~StaffMainWindow()
{
    delete ui;
}
