#ifndef CHECKUPRECORD_H
#define CHECKUPRECORD_H

#include <QWidget>

namespace Ui {
class CheckupRecord;
}

class CheckupRecord : public QWidget
{
    Q_OBJECT

public:
    explicit CheckupRecord(QWidget *parent = nullptr);
    ~CheckupRecord();

private:
    Ui::CheckupRecord *ui;
};

#endif // CHECKUPRECORD_H
