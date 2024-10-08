#ifndef EDITMEDICALEXAMINATIONREPORT_H
#define EDITMEDICALEXAMINATIONREPORT_H

#include <QWidget>
#include <QScrollArea>

#include "clientsocket.h"

namespace Ui {
class EditMedicalExaminationReport;
}

class EditMedicalExaminationReport : public QWidget
{
    Q_OBJECT

public:
    explicit EditMedicalExaminationReport(QWidget *parent = nullptr);
    ~EditMedicalExaminationReport();

    void setCheckupDate(const QJsonArray &checkupDate);

private slots:
    void OnPatientHealthExaminationDateResponce(const QJsonObject& patientDateArray);
private:
    Ui::EditMedicalExaminationReport *ui;
};

#endif // EDITMEDICALEXAMINATIONREPORT_H
