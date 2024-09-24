#include "hepdetails.h"
#include "ui_hepdetails.h"

HEPDetails::HEPDetails(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HEPDetails)
{
    ui->setupUi(this);

    connect(ClientSocket::instance(),&ClientSocket::PackageNameInfo,this,&HEPDetails::setPackageInfo);

    setupSchedule();

    //


}

HEPDetails::~HEPDetails()
{
    delete ui;
}

void HEPDetails::setupSchedule()
{

    // 创建新的布局
    QHBoxLayout *scheduleLayout = new QHBoxLayout();

    // 创建一个新的 QWidget 用于添加日程项目
    QWidget *dateContainer = new QWidget();
    dateContainer->setLayout(scheduleLayout);


    // 在 QScrollArea 中放置这个 QWidget
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(dateContainer);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用垂直滚动条
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded); // 需要时显示水平滚动条

    // 将 scrollArea 添加到主界面
    QVBoxLayout *mainLayout = new QVBoxLayout(ui->scheduleWidget);
    mainLayout->addWidget(scrollArea);

    // 获取当前日期
    QDate currentDate = QDate::currentDate();

    // 生成接下来两周的日期
    for (int i = 0; i < 14; ++i) {
        QDate date = currentDate.addDays(i);
        QWidget *dateItem = new QWidget();
        QVBoxLayout *dateLayout = new QVBoxLayout(dateItem);

        QString dayOfWeek = date.toString("ddd"); // 短格式星期几
        QString dateString = date.toString("MM-dd"); // 月-日

        QLabel *dayLabel = new QLabel(dayOfWeek);
        QLabel *dateLabel = new QLabel(dateString);

        QPushButton *reserveButton = new QPushButton("预约");

        // 设置星期几和日期的样式
        dayLabel->setStyleSheet("font-size: 16pt; background-color: transparent; border: none;"); // 透明背景和无边框
        dateLabel->setStyleSheet("font-size: 12pt; background-color: transparent; border: none;"); // 透明背景和无边框

        // 添加分隔线
        QFrame *line = new QFrame();
        line->setFrameShape(QFrame::HLine); // 水平线
        line->setFrameShadow(QFrame::Sunken);

        // 设置按钮样式和字体大小
        reserveButton->setStyleSheet("QPushButton {"
                                     "background-color: #A9D5E8;"
                                     "color: #333;"
                                     "border: none;"
                                     "padding: 8px;"
                                     "border-radius: 5px;"
                                     "font-size: 12pt;" // 设置按钮字体大小
                                     "}"
                                     "QPushButton:hover {"
                                     "background-color: #A0C4D4;"
                                     "}");

        // 连接按钮的点击信号
        connect(reserveButton, &QPushButton::clicked, this, [this, date]() {
            // 处理预约逻辑
            qDebug() << "预约日期:" << date.toString();
        });

        // 将标签和按钮添加到布局中
        dateLayout->addWidget(dayLabel);
        dateLayout->addWidget(dateLabel);
        dateLayout->addWidget(line); // 添加分隔线
        dateLayout->addWidget(reserveButton);


        dateItem->setLayout(dateLayout);
        scheduleLayout->addWidget(dateItem);
    }
}

void HEPDetails::setCardName(const QString &cardName)
{
    this->m_cardName = cardName;
    ui->packageNameLabel->setText(this->m_cardName);

}

void HEPDetails::setPackageInfo(const QJsonArray &packageInfo, const QString &packageDescription)
{
    // 如果 packageWidget 没有布局，则创建一个新的布局
    if (!ui->packageWidget->layout()) {
        QVBoxLayout *mainLayout = new QVBoxLayout(ui->packageWidget);
        ui->packageWidget->setLayout(mainLayout);
    }

    // 创建滚动区域和内容容器
    QScrollArea *scrollArea = new QScrollArea(this);
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);

    // 设置内容容器的布局
    contentWidget->setLayout(layout);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(contentWidget);

    // 清空之前的内容
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // 创建并添加套餐描述标签
    qDebug()<<"packageDescription="<<packageDescription;
    QLabel *packageDescriptionLabel = new QLabel(packageDescription);
    packageDescriptionLabel->setStyleSheet("color: black; font-weight: bold; font-size: 14pt; background-color: transparent; border: none;");
    layout->addWidget(packageDescriptionLabel);
    layout->addSpacing(50);

    // 创建并添加套餐项目标签
    QLabel *packageItemsLabel = new QLabel("套餐项目");
    packageItemsLabel->setStyleSheet("font-weight: bold; font-size: 16pt; background-color: transparent; border: none;");
    layout->addWidget(packageItemsLabel);
    layout->addSpacing(50);

    // 动态创建项目
    for (const QJsonValue &value : packageInfo) {
        QJsonObject packageObj = value.toObject();

        QLabel *itemNameLabel = new QLabel(packageObj["item_name"].toString());
        QLabel *itemDescriptionLabel = new QLabel(packageObj["item_description"].toString());


        // 设置样式
        itemNameLabel->setStyleSheet("font-weight: bold; font-size: 16pt; color: #003B5C; background-color: transparent; border: none;");
        itemDescriptionLabel->setStyleSheet("font-size: 12pt;background-color: transparent; border: none;");

        layout->addWidget(itemNameLabel);
        layout->addWidget(itemDescriptionLabel);
        layout->addSpacing(10);
    }

    // 清空现有的 packageWidget 内容
    QLayoutItem *existingItem;
    while ((existingItem = ui->packageWidget->layout()->takeAt(0)) != nullptr) {
        delete existingItem->widget();
        delete existingItem;
    }

    // 将滚动区域添加到 packageWidget
    ui->packageWidget->layout()->addWidget(scrollArea);

    // 更新显示
    ui->packageWidget->update();
}
