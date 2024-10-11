#ifndef CHECKUPREPORT_H
#define CHECKUPREPORT_H

#include <QWidget>
#include <QStandardItem>

#include "clientsocket.h"

namespace Ui {
class CheckupReport;
}

class CheckupReport : public QWidget
{
    Q_OBJECT

public:
    explicit CheckupReport(QWidget *parent = nullptr);
    ~CheckupReport();

private slots:
    void OnHealthExaminationRePortListResponce(const QJsonArray & reportsListArray);
    void onViewReportClicked(const QModelIndex &index);
private:
    Ui::CheckupReport *ui;
};

#endif // CHECKUPREPORT_H
