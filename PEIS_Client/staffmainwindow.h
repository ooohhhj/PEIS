#ifndef STAFFMAINWINDOW_H
#define STAFFMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class StaffMainWindow;
}

class StaffMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StaffMainWindow(QWidget *parent = nullptr,const QString &username="");
    ~StaffMainWindow();

private:
    Ui::StaffMainWindow *ui;
    QString m_username;
};

#endif // STAFFMAINWINDOW_H
