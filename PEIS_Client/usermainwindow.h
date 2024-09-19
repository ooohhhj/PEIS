#ifndef USERMAINWINDOW_H
#define USERMAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QTabBar>

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
private:
    Ui::UserMainWindow *ui;
    QString m_username;
};

#endif // USERMAINWINDOW_H
