#ifndef APPOINTMENTMANAGEMENT_H
#define APPOINTMENTMANAGEMENT_H

#include <QWidget>
#include <QDate>
#include <QStandardItemModel>

#include "protocol.h"
#include "clientsocket.h"
#include "delegate.h"

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


private:
    Ui::AppointmentManagement *ui;
    QString m_username;
};

#endif // APPOINTMENTMANAGEMENT_H
