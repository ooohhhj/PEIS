#ifndef PATIENTINFORMATION_H
#define PATIENTINFORMATION_H

#include <QWidget>
#include <QStandardItemModel>
#include <QDateTime>


#include "clientsocket.h"
#include "protocol.h"
#include "delegate.h"

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
    QString convertAppointmentDate(const QString &inputDate);

signals:
    void onEditReportButtonClicked(const QString& patientName);

private:
    Ui::PatientInformation *ui;
};

#endif // PATIENTINFORMATION_H