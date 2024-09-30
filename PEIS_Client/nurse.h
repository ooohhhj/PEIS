#ifndef NURSE_H
#define NURSE_H

#include <QMainWindow>
#include <memory>

#include "appointmentmanagement.h"
#include "patientinformation.h"
#include "inputmedicaexaminationdata.h"

namespace Ui {
class Nurse;
}

class Nurse : public QMainWindow
{
    Q_OBJECT

public:
    explicit Nurse(QWidget *parent = nullptr,const QString &username="");
    ~Nurse();

private slots:
    void on_scheduleCheckupButton_clicked();
    void on_EditReportButtonClicked(const QString&patientName);

private:
    Ui::Nurse *ui;
   QString m_username;
   std::unique_ptr<PatientInformation> patientinfo;
   std::unique_ptr<InputMedicaExaminationData> inputmedicaexaminationdata;
};

#endif // NURSE_H
