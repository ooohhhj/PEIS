#ifndef USERMAINWINDOW_H
#define USERMAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <memory>
#include <QWidget>

#include "clientsocket.h"
#include "schedulecheckup.h"
#include "protocol.h"
#include "hepdetails.h"
#include "checkuprecord.h"
#include "checkupreport.h"
#include "healthexaminationreport.h"

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
    void OnCardClicked(const QString & cardName);
    void setDefaultWidget();
    void buttonStyleSheet();


    void on_healthCheckRecordButton_clicked();

    void on_checkupReportButton_clicked();

    void on_noticeButton_clicked();

    void updatenoticeButton();

    void LookCheckupreport();




private:
    Ui::UserMainWindow *ui;
    QString m_username;
    std::unique_ptr<ScheduleCheckup> scheduleCheckUp ;
    std::unique_ptr<HEPDetails> hepDetails;
    std::unique_ptr<CheckupRecord> checkupRecord;
    std::unique_ptr<CheckupReport> checkupreport;
    std::unique_ptr<HealthExaminationReport> healthexaminationreport;


};

#endif // USERMAINWINDOW_H
