#ifndef APPOINTMENTSMANAGER_NURSE_H
#define APPOINTMENTSMANAGER_NURSE_H

#include <QWidget>
#include <QJsonObject>
#include <QStandardItemModel>

#include "clientsocket.h"

namespace Ui {
class AppointmentsManager_Nurse;
}

class AppointmentsManager_Nurse : public QWidget
{
    Q_OBJECT

public:
    explicit AppointmentsManager_Nurse(QWidget *parent = nullptr);
    ~AppointmentsManager_Nurse();

    void onViewReportClicked(const QModelIndex &index);

    void AppointmentManager(const QString &patientName, const QString &healthPackage,const QString &appointmentDate, const QString &appointmentStatus);

    void OncancelAppointmentButton(const QString &username, const QString &packageName, const QString &appointmentDate);

private slots:

    void showAppointment(const QJsonArray& patientInfoArray);

    void on_searchButton_clicked();

    void on_returnExitButton_clicked();

signals:
    void appointmentsManager_Nurse_search(const QString&patientName);
    void onLookCheckUpReportClicked(const QString & patientName,const QString&healthPackage,const QString&appointmentDate,const int&flag);

    void exitButtonClicked();
private:
    Ui::AppointmentsManager_Nurse *ui;
    QString m_patientName;
};

#endif // APPOINTMENTSMANAGER_NURSE_H
