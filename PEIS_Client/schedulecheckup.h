#ifndef SCHEDULECHECKUP_H
#define SCHEDULECHECKUP_H

#include <QWidget>
#include "clientsocket.h"


namespace Ui {
class ScheduleCheckup;
}

class ScheduleCheckup : public QWidget
{
    Q_OBJECT

public:
    explicit ScheduleCheckup(QWidget *parent = nullptr);
    ~ScheduleCheckup();


    void requestPackagesForPage(int page);//请求指定页码的套餐数据

    int getCurrentPage();

    int getItemsPerPage();

    void reserveCheckup(const QJsonArray & packagesArray ,const int& totalPage);

    void setUsername(const QString & username);

private slots:
    void on_nextBtn_clicked();

    void on_prevBtn_clicked();

    void on_cardNameButton_clicked();
    void on_cardNameButton_2_clicked();

    void on_cardNameButton_3_clicked();

    void on_pushButton_clicked();

    void on_returnExitButton_clicked();

    void on_reflushButton_clicked();

    void on_AppointmentButton_clicked();

    void onGetAppointmentInfo(const QString &username,const QString &packageName,const QString &appointmentDate,const QString&status);

    void OncancelAppointmentButton(const QString &username, const QString &packageName, const QString &appointmentDate);

signals:
    void cardClicked(const QString & cardName);
    void exitButtonClicked();

private:
    Ui::ScheduleCheckup *ui;
    int currentPage ;
    const int itemsPerPage ;
    int searchFlag;
    QString m_username;
};

#endif // SCHEDULECHECKUP_H
