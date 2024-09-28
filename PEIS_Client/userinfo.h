#ifndef USERINFO_H
#define USERINFO_H

#include <QWidget>
#include <QStandardItemModel>


#include "protocol.h"
#include "clientsocket.h"

namespace Ui {
class UserInfo;
}

class UserInfo : public QWidget
{
    Q_OBJECT

public:
    explicit UserInfo(QWidget *parent = nullptr);
    ~UserInfo();

private slots:
      void OnAddUserInfos(const QJsonArray &userInfosArray);
private:
    Ui::UserInfo *ui;
};

#endif // USERINFO_H
