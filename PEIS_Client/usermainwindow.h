#ifndef USERMAINWINDOW_H
#define USERMAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <memory>
#include <QWidget>

#include "clientsocket.h"
#include "schedulecheckup.h"
#include "protocol.h"
#include "hepdetails.h"

namespace Ui {
class UserMainWindow;
}

class UserMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserMainWindow(QWidget *parent = nullptr,const QString &username="");
    ~UserMainWindow();

    void setUsername(const QString& username );

private slots:
    void on_scheduleCheckupButton_clicked();
    void OnCardClicked(const QString & cardName);
    void setDefaultWidget();
    void buttonStyleSheet();


private:
    Ui::UserMainWindow *ui;
    QString m_username;
    std::unique_ptr<ScheduleCheckup> scheduleCheckUp ;
    std::unique_ptr<HEPDetails> hepDetails;


};

#endif // USERMAINWINDOW_H
