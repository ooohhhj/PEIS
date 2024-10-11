#ifndef HEALTHEXAMINATIONREPORT_H
#define HEALTHEXAMINATIONREPORT_H

#include <QWidget>

namespace Ui {
class HealthExaminationReport;
}

class HealthExaminationReport : public QWidget
{
    Q_OBJECT

public:
    explicit HealthExaminationReport(QWidget *parent = nullptr);
    ~HealthExaminationReport();

private:
    Ui::HealthExaminationReport *ui;
};

#endif // HEALTHEXAMINATIONREPORT_H
