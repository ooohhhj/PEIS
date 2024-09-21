#ifndef SCHEDULECHECKUP_H
#define SCHEDULECHECKUP_H

#include <QWidget>

namespace Ui {
class ScheduleCheckup;
}

class ScheduleCheckup : public QWidget
{
    Q_OBJECT

public:
    explicit ScheduleCheckup(QWidget *parent = nullptr);
    ~ScheduleCheckup();

private:
    Ui::ScheduleCheckup *ui;
};

#endif // SCHEDULECHECKUP_H
