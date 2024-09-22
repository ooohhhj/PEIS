#include "schedulecheckup.h"
#include "ui_schedulecheckup.h"


ScheduleCheckup::ScheduleCheckup(QWidget *parent) :
    QWidget(parent),currentPage(1),itemsPerPage(3),
    ui(new Ui::ScheduleCheckup)
{
    ui->setupUi(this);


    this->setStyleSheet("border: 2px solid #B0C4DE;"  // 边框为银灰色
                        "border-radius: 13px;"        // 圆角，内角半径为15px
                        "box-shadow: inset 4px 4px 10px rgba(0, 0, 0, 0.1);");


    connect(ClientSocket::instance(),&ClientSocket::ReserveCheckup,this,&ScheduleCheckup::reserveCheckup);
}

ScheduleCheckup::~ScheduleCheckup()
{
    delete ui;
}

void ScheduleCheckup::requestPackagesForPage(int page)
{
    QJsonObject info;
    info["page"] =page;// 当前页码
    info["itemsPerPage"] = itemsPerPage;// 每页显示数量

    //封包分页查询请求
    Packet packageQueryPacket = Protocol::createPacket(UpdateCheckupPackageRequest,info);

    QByteArray dataToSend =Protocol::serializePacket(packageQueryPacket);

    ClientSocket::instance()->senData(dataToSend);

}

int ScheduleCheckup::getCurrentPage()
{
    return this->currentPage;
}

int ScheduleCheckup::getItemsPerPage()
{
    return this->itemsPerPage;
}

void ScheduleCheckup::reserveCheckup(const QJsonArray &packagesArray , const int &totalPage)
{

    //设置图标和文本
    // 更新卡片1内容
    if (packagesArray.size() > 0) {
        QJsonObject package1 = packagesArray[0].toObject();
        ui->cardNameButton->setText(package1["package_name"].toString());
        ui->TALabel->setText(package1["target_population"].toString());
        ui->HCELabel->setText(package1["provider"].toString());
        ui->PriceLabel->setText(QString::number(package1["price"].toDouble()));
        ui->PriceLabel->setScaledContents(true);

    }

    // 更新卡片2内容
    if (packagesArray.size() > 1) {
        QJsonObject package2 = packagesArray[1].toObject();
        ui->cardNameButton_2->setText(package2["package_name"].toString());
        ui->TALabel_2->setText(package2["target_population"].toString());
        ui->HCELabel_2->setText(package2["provider"].toString());
        ui->PriceLabel_2->setText(QString::number(package2["price"].toDouble()));
        ui->PriceLabel_2->setScaledContents(true);

    }

    // 更新卡片3内容
    if (packagesArray.size() > 2) {
        QJsonObject package3 = packagesArray[2].toObject();
        ui->cardNameButton_3->setText(package3["package_name"].toString());
        ui->TALabel_3->setText(package3["target_population"].toString());
        ui->HCELabel_3->setText(package3["provider"].toString());
        ui->PriceLabel_3->setText(QString::number(package3["price"].toDouble()));
        ui->PriceLabel_3->setScaledContents(true);
    }

    ui->totalPageLabel->setText(QString::number(totalPage));
    ui->curPageLabel ->setText(QString::number(this->currentPage));

    if(this->currentPage==1 )
    {
        ui->prevBtn->setVisible(false);
    }

    if(this->currentPage == totalPage)
    {
        ui->nextBtn->setVisible(false);
    }




}

void ScheduleCheckup::on_nextBtn_clicked()
{
    currentPage++;
    requestPackagesForPage(currentPage);
}


void ScheduleCheckup::on_prevBtn_clicked()
{
    if(currentPage>1)
    {
        currentPage --;
        requestPackagesForPage(currentPage);
    }
}

