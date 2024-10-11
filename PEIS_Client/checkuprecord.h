#ifndef CHECKUPRECORD_H
#define CHECKUPRECORD_H

#include <QWidget>
#include <QStandardItem>

#include "clientsocket.h"


namespace Ui {
class CheckupRecord;
}

class CheckupRecord : public QWidget
{
    Q_OBJECT

public:
    explicit CheckupRecord(QWidget *parent = nullptr);
    ~CheckupRecord();

     void setUsername(const QString & username);

private slots:
    void OnHealthExaminationRecordsResponce(const QJsonArray& recordsArray);
    void onViewReportClicked(const QModelIndex &index);
    void on_returnBtn_clicked();

signals:
    void exitButtonClicked();
    void LookCheckupreport();
private:
    Ui::CheckupRecord *ui;
    QString m_username;
};

#endif // CHECKUPRECORD_H
