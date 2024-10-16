#ifndef STAFFMAINWINDOW_H
#define STAFFMAINWINDOW_H

#include <QMainWindow>
#include <memory>

#include "appointmentmanagement.h"
#include "editmedicalexaminationreport.h"
#include "electronicmedicalrecord.h"
#include "healthexaminationreport.h"

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


    void on_noticeButton_clicked();
    void updatenoticeButton();

    void on_checkupReportButton_clicked();

    void OnEditCheckuppreport(const QString & patientName,const QString&packageName,const QString & appointmentDate);

    void onLookCheckUpReportClicked(const QString & patientName,const QString&healthPackage,const QString&appointmentDate);

    void setDefaultWidget();

    void healthexaminationreport_exitButtonClicked();

    void editmedicalexaminationreport_exitButtonClicked();

private:
    Ui::StaffMainWindow *ui;
    QString m_username;
    std::unique_ptr<AppointmentManagement> appointmentmanagement;
    std::unique_ptr<EditMedicalExaminationReport> editmedicalexaminationreport;
    std::unique_ptr<ElectronicMedicalRecord> electronicmedicalrecord;
    std::unique_ptr<HealthExaminationReport> healthexaminationreport;
};

#endif // STAFFMAINWINDOW_H
