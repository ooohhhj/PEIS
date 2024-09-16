#ifndef PASSWORDRECOVERYDIALOG_H
#define PASSWORDRECOVERYDIALOG_H

#include <QDialog>

namespace Ui {
class PasswordRecoveryDialog;
}

class PasswordRecoveryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordRecoveryDialog(QWidget *parent = nullptr);
    ~PasswordRecoveryDialog();

private:
    Ui::PasswordRecoveryDialog *ui;
};

#endif // PASSWORDRECOVERYDIALOG_H
