#ifndef INPUTMEDICAEXAMINATIONDATA_H
#define INPUTMEDICAEXAMINATIONDATA_H

#include <QWidget>
#include <QDebug>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>


#include "clientsocket.h"

namespace Ui {
class InputMedicaExaminationData;
}

class InputMedicaExaminationData : public QWidget
{
    Q_OBJECT

public:
    explicit InputMedicaExaminationData(QWidget *parent = nullptr);
    ~InputMedicaExaminationData();

    void setPatientInfo(const QString &patientName, const QString &patientGender, const QString &patientPhone, const QString &patientBirthDate, const QString &healthPackage, const QString &appointmentDate, const QString &appointmentStatus);

private slots:
    void onHealthCheckupItemResponce(const QJsonArray & packageItems);
    void on_submitButton_clicked();
    void on_OnRecordHealthCheckupResponce(const QJsonArray & checkupDate);

    void on_returnExitButton_clicked();

signals:
    void exitButtonClicked();

private:
    Ui::InputMedicaExaminationData *ui;

};

#endif // INPUTMEDICAEXAMINATIONDATA_H
