#ifndef USERMAINWINDOW_H
#define USERMAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <memory>

#include "schedulecheckup.h"

namespace Ui {
class UserMainWindow;
}

class UserMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserMainWindow(QWidget *parent = nullptr,const QString &username="");
    ~UserMainWindow();

    void setUsername(const QString& username );
private slots:
    void on_scheduleCheckupButton_clicked();

private:
    Ui::UserMainWindow *ui;
    QString m_username;
    std::unique_ptr<ScheduleCheckup> scheduleCheckUp ;


};

#endif // USERMAINWINDOW_H
