#ifndef APPOINTMENTMANAGEMENT_H
#define APPOINTMENTMANAGEMENT_H

#include <QWidget>
#include <QDate>
#include <QStandardItemModel>
#include <QPushButton>
#include <QScrollArea>

#include "protocol.h"
#include "clientsocket.h"


namespace Ui {
class AppointmentManagement;
}

class AppointmentManagement : public QWidget
{
    Q_OBJECT

public:
    explicit AppointmentManagement(QWidget *parent = nullptr);
    ~AppointmentManagement();


    void setUsername(const QString &username);


private slots:
    void setDateQComBOx(const QString &startDate);
    void OnAppointmentsDate(const QJsonArray & appointments);
    void onViewReportClicked(const QModelIndex &index);

    void on_returnExitButton_clicked();

signals:
    void EditCheckuppreport(const QString & patientName,const QString&packageName,const QString & appointmentDate,const int&flag);
    void exitButtonClicked();
    void onLookCheckUpReportClicked(const QString & patientName,const QString&healthPackage,const QString&appointmentDate,const int&flag);

private:
    Ui::AppointmentManagement *ui;
    QString m_username;
};

#endif // APPOINTMENTMANAGEMENT_H
