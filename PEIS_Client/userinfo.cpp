#include "userinfo.h"
#include "ui_userinfo.h"

UserInfo::UserInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserInfo)
{
    ui->setupUi(this);

    connect(ClientSocket::instance(),&ClientSocket::userInfos,this,&UserInfo::OnAddUserInfos);

}

UserInfo::~UserInfo()
{
    delete ui;
}



void UserInfo::OnAddUserInfos(const QJsonArray &userInfosArray)
{
    QStandardItemModel * model =new QStandardItemModel(this);

    // 设置列标题
    model->setHorizontalHeaderLabels({"ID", "用户名", "性别", "出生日期", "身份证", "地址", "手机号", "角色", "注册时间", "更新时间"});
    // 遍历 QJsonArray，将每个用户信息插入到模型中
    for (int i = 0; i < userInfosArray.size(); ++i) {
        QJsonObject userObject = userInfosArray[i].toObject();

        // 在模型中增加新行
        QList<QStandardItem *> rowItems;
        rowItems << new QStandardItem(QString::number(userObject["id"].toInt()));
        rowItems << new QStandardItem(userObject["username"].toString());
        rowItems << new QStandardItem(userObject["gender"].toString());
        rowItems << new QStandardItem(userObject["birth_date"].toString());
        rowItems << new QStandardItem(userObject["id_card"].toString());
        rowItems << new QStandardItem(userObject["address"].toString());
        rowItems << new QStandardItem(userObject["phone_number"].toString());
        rowItems << new QStandardItem(userObject["role"].toString());
        rowItems << new QStandardItem(userObject["created_at"].toString());
        rowItems << new QStandardItem(userObject["updated_at"].toString());

        // 将行数据添加到模型中
        model->appendRow(rowItems);
    }

    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
