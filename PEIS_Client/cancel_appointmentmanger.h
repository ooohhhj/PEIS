#ifndef CANCEL_APPOINTMENTMANGER_H
#define CANCEL_APPOINTMENTMANGER_H

#include <QWidget>
#include <QStandardItem>

#include "clientsocket.h"


namespace Ui {
class Cancel_AppointmentManger;
}

class Cancel_AppointmentManger : public QWidget
{
    Q_OBJECT

public:
    explicit Cancel_AppointmentManger(QWidget *parent = nullptr);
    ~Cancel_AppointmentManger();
    void onViewReportClicked(const QModelIndex &index);

    void AppointmentManager(const QString &patientName, const QString &healthPackage,const QString &appointmentDate, const QString &appointmentStatus);

    void OncancelAppointmentButton(const QString &username, const QString &packageName, const QString &appointmentDate);

private slots:
    void showAppointment(const QJsonArray& patientInfoArray);


signals:
     void onLookCheckUpReportClicked(const QString & patientName,const QString&healthPackage,const QString&appointmentDate);
private:
    Ui::Cancel_AppointmentManger *ui;
};

#endif // CANCEL_APPOINTMENTMANGER_H
