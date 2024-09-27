#ifndef CHECKUPREPORT_H
#define CHECKUPREPORT_H

#include <QWidget>

namespace Ui {
class CheckupReport;
}

class CheckupReport : public QWidget
{
    Q_OBJECT

public:
    explicit CheckupReport(QWidget *parent = nullptr);
    ~CheckupReport();

private:
    Ui::CheckupReport *ui;
};

#endif // CHECKUPREPORT_H
