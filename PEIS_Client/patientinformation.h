#ifndef PATIENTINFORMATION_H
#define PATIENTINFORMATION_H

#include <QWidget>
#include <QStandardItemModel>
#include <QDateTime>


#include "clientsocket.h"
#include "protocol.h"


namespace Ui {
class PatientInformation;
}

class PatientInformation : public QWidget
{
    Q_OBJECT

public:
    explicit PatientInformation(QWidget *parent = nullptr);
    ~PatientInformation();

private slots:
    void on_searchButton_clicked();
    void OnPatientInfo(const QJsonArray & patientInfoArray);

    void onViewReportClicked(const QModelIndex &index);




    void on_returnExitButton_clicked();

signals:
    void onEditReportButtonClicked(const QString& patientName,const QString &patientGender,const QString &patientPhone,
                                   const QString&patientBirthDate,const QString&healthPackage,const QString&appointmentDate,
                                   const QString&appointmentStatus,const int& flag);

    void onLookCheckUpReportClicked(const QString & patientName,const QString&healthPackage,const QString&appointmentDate,const int& flag);

    void exitButtonClicked();
private:
    Ui::PatientInformation *ui;
};

#endif // PATIENTINFORMATION_H
