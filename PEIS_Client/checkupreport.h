#ifndef CHECKUPREPORT_H
#define CHECKUPREPORT_H

#include <QWidget>
#include <QStandardItem>

#include "clientsocket.h"
#include "protocol.h"

namespace Ui {
class CheckupReport;
}

class CheckupReport : public QWidget
{
    Q_OBJECT

public:
    explicit CheckupReport(QWidget *parent = nullptr);
    ~CheckupReport();

    void setUsername(const QString & username);

private slots:
    void OnHealthExaminationRePortListResponce(const QJsonArray & reportsListArray);
    void onViewReportClicked(const QModelIndex &index);
    void on_returnBtn_clicked();

signals:
    void exitButtonClicked();
    void LookCheckupreport(const int & flag);

private:
    Ui::CheckupReport *ui;
    QString m_username;
};

#endif // CHECKUPREPORT_H
