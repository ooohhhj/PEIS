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

private slots:
    void OnHealthExaminationRecordsResponce(const QJsonArray& recordsArray);
    void onViewReportClicked(const QModelIndex &index);
private:
    Ui::CheckupRecord *ui;
};

#endif // CHECKUPRECORD_H
