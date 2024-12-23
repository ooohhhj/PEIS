#ifndef NURSE_H
#define NURSE_H

#include <QMainWindow>
#include <memory>
#include<QTableView>

#include "appointmentmanagement.h"
#include "patientinformation.h"
#include "inputmedicaexaminationdata.h"
#include "protocol.h"
#include "healthexaminationreport.h"
#include "appointmentsmanager_nurse.h"


namespace Ui {
class Nurse;
}

class Nurse : public QMainWindow
{
    Q_OBJECT

public:
    explicit Nurse(QWidget *parent = nullptr,const QString &username="");
    ~Nurse();

    void buttonStyleSheet(QPushButton *button);
private slots:
    void on_EditReportButtonClicked(const QString&patientName,const QString &patientGender,const QString &patientPhone,
                                    const QString&patientBirthDate,const QString&healthPackage,const QString&appointmentDate,
                                    const QString&appointmentStatus,const int & flag);

    void onLookCheckUpReportClicked(const QString & patientName,const QString&healthPackage,const QString&appointmentDate,const int& flag);

    void exitButtonClicked(const int& flag);


    void InputMedicaExaminationData_exitButtonClicked();

    void setDefaultWidget();
    void on_patientInfoButton_clicked();

    void on_appointmentButton_clicked();


    void on_checkupReportButton_clicked();

private:
    Ui::Nurse *ui;
    QString m_username;
    std::unique_ptr<PatientInformation> patientinfo;
    std::unique_ptr<InputMedicaExaminationData> inputmedicaexaminationdata;
    std::unique_ptr<HealthExaminationReport> healthexaminationreport;
    std::unique_ptr<AppointmentsManager_Nurse> appointmentsmanager_nurse;
};

#endif // NURSE_H
