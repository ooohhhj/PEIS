#ifndef PASSWORDRECOVERYDIALOG_H
#define PASSWORDRECOVERYDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QJsonObject>
#include "protocol.h"
#include "clientsocket.h"
#include "captchagenerator.h"


namespace Ui {
class PasswordRecoveryDialog;
}

class PasswordRecoveryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordRecoveryDialog(QWidget *parent = nullptr);
    ~PasswordRecoveryDialog();

private slots:
    void validatePhoneNumber(const QString &text);
    void onCaptchaChanged(const QString &text);
    void passwordLineEdit(const QString &text);
    void onConfirmPasswordChanged(const QString &text);


    void on_reflushButton_clicked();

    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void NotValidatePhoneNumber(const QString & text);


private:
    Ui::PasswordRecoveryDialog *ui;
    QVector<bool> inputDateFlag;
};

#endif // PASSWORDRECOVERYDIALOG_H
