#include "passwordrecoverydialog.h"
#include "ui_passwordrecoverydialog.h"

PasswordRecoveryDialog::PasswordRecoveryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordRecoveryDialog)
{
    ui->setupUi(this);
}

PasswordRecoveryDialog::~PasswordRecoveryDialog()
{
    delete ui;
}
