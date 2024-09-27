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
    explicit StaffMainWindow(QWidget *parent = nullptr);
    ~StaffMainWindow();

private:
    Ui::StaffMainWindow *ui;
};

#endif // STAFFMAINWINDOW_H
