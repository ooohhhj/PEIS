#ifndef STAFFMAINWINDOW_H
#define STAFFMAINWINDOW_H

#include <QMainWindow>
#include <memory>

#include "appointmentmanagement.h"

namespace Ui {
class StaffMainWindow;
}

class StaffMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StaffMainWindow(QWidget *parent = nullptr,const QString &username="");
    ~StaffMainWindow();

    void OnStartDateRequest();

private slots:
    void on_healthCheckRecordButton_clicked();


private:
    Ui::StaffMainWindow *ui;
    QString m_username;
    std::unique_ptr<AppointmentManagement> appointmentmanagement;

};

#endif // STAFFMAINWINDOW_H
