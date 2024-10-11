#ifndef HEALTHEXAMINATIONREPORT_H
#define HEALTHEXAMINATIONREPORT_H

#include <QWidget>
#include <poppler-qt5.h>
#include <QFile>
#include <QScrollArea>


#include "clientsocket.h"

namespace Ui {
class HealthExaminationReport;
}

class HealthExaminationReport : public QWidget
{
    Q_OBJECT

public:
    explicit HealthExaminationReport(QWidget *parent = nullptr);
    ~HealthExaminationReport();

    void displayPdf(const QString &fileName);

private slots:
    void on_returnBtn_clicked();
    void OnGetHealthExaminationRePortResponce(const QJsonObject& reprotPdfDate);

signals:
    void exitButtonClicked();
private:
    Ui::HealthExaminationReport *ui;
};

#endif // HEALTHEXAMINATIONREPORT_H
