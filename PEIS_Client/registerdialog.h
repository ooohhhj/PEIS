#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QDate>
#include <QCalendarWidget>
#include <QComboBox>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

    void setBirthDate();
private slots:
    void onYearChanged(int);
    void onMonthChanged(int);
    void updateDaysComboBox(QComboBox * yearComboBox,QComboBox * monthComboBox);
private:
    Ui::RegisterDialog *ui;
};

#endif // REGISTERDIALOG_H
