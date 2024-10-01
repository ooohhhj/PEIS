#include "hepdetails.h"
#include "ui_hepdetails.h"

HEPDetails::HEPDetails(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HEPDetails)
{
    ui->setupUi(this);

    connect(ClientSocket::instance(),&ClientSocket::PackageNameInfo,this,&HEPDetails::setPackageInfo);

    setupSchedule();

    setExamination();

    setInstructions("H:/PEIS/PEIS/examination_notice.json");


    connect(ClientSocket::instance(),&ClientSocket::updateUserAppointment,this,&HEPDetails::OnUpdateUserAppointment);


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

        // 创建日期字符串，用于发送给服务器
        QString fullDateString = date.toString("yyyy-MM-dd"); // 完整日期格式


        QLabel *dayLabel = new QLabel(dayOfWeek);
        QLabel *dateLabel = new QLabel(dateString);

        QPushButton *reserveButton = new QPushButton("预约");
        // 设置按钮的对象名称为完整日期字符串  用于后面点击事件
        reserveButton->setObjectName(fullDateString);


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
        connect(reserveButton, &QPushButton::clicked, this, &HEPDetails::on_reserveButton_clicked);

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

void HEPDetails::setExamination()
{
    QJsonObject jsonObj;

    jsonObj["体检须知"] = QJsonObject{
    {"一、体检前", QJsonArray{
            "1. 体检中心抽血时间为周一到周六上午 7:30-11:00 (11:00 后不抽血)。",
            "2. 如需开单位发票，请先提前电话咨询体检中心，开票成功不可撤换。",
            "3. 体检前三天，请您清淡饮食，勿饮酒，勿疲劳，体检当日禁食早餐，避免剧烈运动。",
            "4. 体检时请您携带体检通知单及身份证件，男士在做膀胱、前列腺彩超前，请憋尿，女士在月经期请勿做妇科及尿液检查。孕妇不可做 X 光类检查。",
            "5. 如有携带贵重物品和现金的，请妥善保管。",
            "6. 行动不便及年满 75 岁以上的体检者，请家属陪同体检。",
            "7. 教师资格证、护士等带有特殊要求的人员需按要求携带相关证件及照片。"
}},
    {"二、体检中", QJsonArray{
            "1. 检查项目中腹部彩超、静脉采血、肝胆彩超等项目须空腹完成。",
            "2. 请女士注意检查项目的时间安排，避免与孕期检查冲突。",
            "3. 放射检查前请摘除金属饰品和隐形眼镜。"
}},
    {"三、体检后", QJsonArray{
            "1. 体检报告一般发放时间为每天 15:00-17:30。",
            "2. 根据医生建议，可复查或随诊。",
            "3. 可以通过体检中心微信号查询报告。"
}}
};

    // 获取当前项目路径
    QString filePath = "H:/PEIS/PEIS/examination_notice.json";

    // 将 JSON 对象写入文件
    QJsonDocument jsonDoc(jsonObj);
    QFile jsonFile(filePath);
    if (jsonFile.open(QIODevice::WriteOnly)) {
        jsonFile.write(jsonDoc.toJson());
        jsonFile.close();
    }
}

void HEPDetails::setInstructions(const QString &filePath)
{
    // 读取 JSON 文件
    QFile jsonFile(filePath);

    if (!jsonFile.open(QIODevice::ReadOnly)) {
        qWarning("无法打开文件进行读取: %s", qPrintable(filePath));
        return;
    }

    QByteArray jsonData = jsonFile.readAll();
    jsonFile.close();

    QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonData));
    QJsonObject instructions = jsonDoc.object().value("体检须知").toObject();

    // 清空现有内容并创建新的布局
    if (!ui->instructionsWidget->layout()) {
        ui->instructionsWidget->setLayout(new QVBoxLayout());
    } else {
        QLayoutItem *item;
        while ((item = ui->instructionsWidget->layout()->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    }

    // 创建 QTextEdit 用于显示内容
    QTextEdit *textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true); // 设置为只读
    textEdit->setStyleSheet("background-color: transparent; "); // 设置背景透明和字体大小

    QString allInstructions;

    // 按顺序添加注意事项
    QStringList keys = { "一、体检前", "二、体检中", "三、体检后" };
    for (const QString &key : keys) {
        if (instructions.contains(key)) {
            allInstructions += QString("<span style='color: #003B5C; font-weight: bold; font-size: 16pt;'>%1</span><br>").arg(key); // 设置键的样式
            allInstructions += "<br>"; // 添加间隔
            QJsonArray items = instructions[key].toArray();
            for (const QJsonValue &item : items) {
                allInstructions += QString("<span style='font-size: 11pt;'>- %1</span><br>").arg(item.toString()); // 添加项目并设置样式
            }
            allInstructions += "<br>"; // 添加间隔
        }
    }


    textEdit->setHtml(allInstructions); // 设置 HTML 格式的文本

    // 创建滚动区域
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(textEdit); // 将 QTextEdit 设置为滚动区域的内容

    // 添加滚动区域到布局
    ui->instructionsWidget->layout()->addWidget(scrollArea);
}

void HEPDetails::requestPackageInfo()
{
    //发送获取套餐请求
    QJsonObject info;
    info["cardName"] = this->m_cardName;

    Packet packet =Protocol::createPacket(GetCheckupPackageCountRequest,info);
    QByteArray dataToSend = Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(dataToSend);
}

void HEPDetails::setUsername(const QString &username)
{
    this->m_username =username;
}

void HEPDetails::on_exitButton_clicked()
{

    //返回套餐页
    emit exitButtonClicked();

}

void HEPDetails::on_reserveButton_clicked()
{
    QPushButton * button =qobject_cast<QPushButton*>(sender());
    if(!button) return;

    //获取与按钮相关联的日期
    QString selectedDate = button->objectName();// 获取按钮对象名称（即日期字符串）

    //发送获取套餐请求
    QJsonObject info;
    info["username"] =this->m_username;
    info["cardName"] = this->m_cardName;
    info["selectDate"] =selectedDate;

    Packet packet =Protocol::createPacket(GetCheckupPackageCountRequest,info);
    QByteArray dataToSend = Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(dataToSend);
}

void HEPDetails::OnUpdateUserAppointment(const QString &selectdate)
{
    // 获取父控件中的所有子控件
    QList<QPushButton*> buttons = ui->scheduleWidget->findChildren<QPushButton*>();

    // 遍历每个按钮并检查它的 objectName
    for (QPushButton* button : buttons) {
        if (button->objectName() == selectdate) {
            qDebug() << "Found button with name 'selectdate'";
            button->setText("已预约");
            button->setStyleSheet("background-color: transparent;border: none;");
            break;
            // 执行相关操作
        } else {
            qDebug() << "Button name: " << button->objectName();
        }
    }
}

