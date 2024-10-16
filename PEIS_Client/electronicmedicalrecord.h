#ifndef ELECTRONICMEDICALRECORD_H
#define ELECTRONICMEDICALRECORD_H

#include <QWidget>
#include <QDate>
#include <QStandardItemModel>
#include <QPushButton>

#include "protocol.h"
#include "clientsocket.h"

namespace Ui {
class ElectronicMedicalRecord;
}

class ElectronicMedicalRecord : public QWidget
{
    Q_OBJECT

public:
    explicit ElectronicMedicalRecord(QWidget *parent = nullptr);
    ~ElectronicMedicalRecord();
    void setUsername(const QString & username);
private slots:
     void OnAppointmentsDate(const QJsonArray & appointments);
       void onViewReportClicked(const QModelIndex &index);

private:
    Ui::ElectronicMedicalRecord *ui;
    QString  m_username;
};

#endif // ELECTRONICMEDICALRECORD_H
