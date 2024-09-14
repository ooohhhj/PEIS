#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QDate>
#include <QCalendarWidget>
#include <QComboBox>
#include "captchagenerator.h"
#include <QDebug>


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
    void on_refreshCaptchButton_clicked();

    void on_okButton_clicked();

    void onUsernameChanged(const QString &text);
    void validateIdNumber(const QString &text);
    void validatePhoneNumber(const QString &text);
    void onPasswordChanged(const QString &text);
    void onConfirmPasswordChanged(const QString &text);
    void onCaptchaChanged(const QString &text);

private:
    Ui::RegisterDialog *ui;
};

#endif // REGISTERDIALOG_H
