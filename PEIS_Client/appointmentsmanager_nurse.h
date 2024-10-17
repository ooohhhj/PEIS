#ifndef APPOINTMENTSMANAGER_NURSE_H
#define APPOINTMENTSMANAGER_NURSE_H

#include <QWidget>
#include <QJsonObject>

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

private slots:
    void on_searchButton_clicked();


    void on_returnExitButton_clicked();

signals:
    void appointmentsManager_Nurse_search(const QString&patientName);

    void exitButtonClicked();
private:
    Ui::AppointmentsManager_Nurse *ui;
};

#endif // APPOINTMENTSMANAGER_NURSE_H
