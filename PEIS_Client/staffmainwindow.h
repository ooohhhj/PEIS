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

    void buttonStyleSheet(QPushButton *button);

private slots:

    void on_noticeButton_clicked();
    void updatenoticeButton();

    void OnEditCheckuppreport(const QString & patientName,const QString&packageName,const QString & appointmentDate);

    void onLookCheckUpReportClicked(const QString & patientName,const QString&healthPackage,const QString&appointmentDate,const int&flag);

    void setDefaultWidget();

    void healthexaminationreport_exitButtonClicked(const int&flag);

    void editmedicalexaminationreport_exitButtonClicked();

    void on_AppointmentManagerButton_clicked();

    void on_electronicmedicalrecordButton_clicked();

private:
    Ui::StaffMainWindow *ui;
    QString m_username;
    std::unique_ptr<AppointmentManagement> appointmentmanagement;
    std::unique_ptr<EditMedicalExaminationReport> editmedicalexaminationreport;
    std::unique_ptr<ElectronicMedicalRecord> electronicmedicalrecord;
    std::unique_ptr<HealthExaminationReport> healthexaminationreport;
};

#endif // STAFFMAINWINDOW_H
