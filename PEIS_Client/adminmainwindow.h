#ifndef ADMINMAINWINDOW_H
#define ADMINMAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QDebug>
#include <memory>

#include "userinfo.h"


namespace Ui {
class AdminMainWindow;
}

class AdminMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminMainWindow(QWidget *parent = nullptr,const QString &username="");
    ~AdminMainWindow();

private slots:
    void OnUserItemClicked(QTreeWidgetItem* item,int column);


private:
    Ui::AdminMainWindow *ui;
    QString m_username;
    std::unique_ptr<UserInfo> userinfo;
};

#endif // ADMINMAINWINDOW_H
