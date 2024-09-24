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

private slots:
    void on_nextBtn_clicked();

    void on_prevBtn_clicked();

    void on_cardNameButton_clicked();
signals:
    void cardClicked(const QString & cardName);

private:
    Ui::ScheduleCheckup *ui;
    int currentPage ;
    const int itemsPerPage ;
};

#endif // SCHEDULECHECKUP_H
