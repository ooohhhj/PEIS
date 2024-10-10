#include "checkuprecord.h"
#include "ui_checkuprecord.h"

CheckupRecord::CheckupRecord(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CheckupRecord)
{
    ui->setupUi(this);

    connect(ClientSocket::instance(),&ClientSocket::OnHealthExaminationRecordsResponce,this,&CheckupRecord::OnHealthExaminationRecordsResponce);
}

CheckupRecord::~CheckupRecord()
{
    delete ui;
}

void CheckupRecord::OnHealthExaminationRecordsResponce(const QJsonArray &recordsArray)
{
    qDebug()<<"hhh";
    // 清空当前模型
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"套餐名称", "日期", "预约状态", "操作"});

    // 设置表头字体
    QFont headerFont = ui->tableView->horizontalHeader()->font();
    headerFont.setPointSize(16); // 设置字体大小
    ui->tableView->horizontalHeader()->setFont(headerFont);

    // 添加数据到模型
    for (const QJsonValue &value : recordsArray) {
        QJsonObject appointment = value.toObject();

        QString packageName = appointment["health_package"].toString();
        QString appointmentDate = appointment["appointment_date"].toString();
        QString status = appointment["appointment_status"].toString();

        // 仅在状态为 "已体检" 时添加到模型
        if (status == "已完成") {
            QList<QStandardItem *> rowItems;
            rowItems << new QStandardItem(packageName)
                     << new QStandardItem(appointmentDate)
                     << new QStandardItem(status)
                     << new QStandardItem();

            model->appendRow(rowItems);
        }
    }

    if (model->rowCount() > 0) {
        ui->tableView->setModel(model);

        // 添加按钮到 "操作" 列
        for (int row = 0; row < model->rowCount(); ++row) {
            QString status = model->item(row, 2)->text(); // 获取预约状态

            QPushButton *button = new QPushButton();
            if (status == "已完成") {
                button->setText("查看报告");
            } else {
                button->setText("未知操作");
            }

            // 设置按钮样式
            button->setStyleSheet(
                        "QPushButton {"
                        "   background-color: #A2D8D0;"
                        "   color: #004D40;"
                        "   border: 2px solid #78C1B1;"
                        "   padding: 8px 16px;"
                        "   font-size: 16px;"
                        "   font-weight: bold;"
                        "   border-radius: 12px;"
                        "}"
                        "QPushButton:hover {"
                        "   background-color: #87C5BB;"
                        "   border: 2px solid #5DA49D;"
                        "}"
                        "QPushButton:pressed {"
                        "   background-color: #5DA49D;"
                        "   border: 2px solid #468882;"
                        "}"
                        );

            // 将按钮设置到表格中
            ui->tableView->setIndexWidget(model->index(row, 3), button);

            // 连接按钮点击信号到槽函数
            connect(button, &QPushButton::clicked, this, [this, model, row]() {
                QModelIndex index = model->index(row, 0);
                onViewReportClicked(index);
            });
        }
    } else {
        // 如果没有记录，添加标签到 tableView
        QLabel *noRecordsLabel = new QLabel("你暂时没有体检记录");
        noRecordsLabel->setAlignment(Qt::AlignCenter);
        noRecordsLabel->setStyleSheet("font-size: 18px; color: #FF0000;"); // 设置标签样式

        // 清空模型并设置标签
        ui->tableView->setModel(nullptr); // 清空模型
        ui->tableView->setIndexWidget(model->index(0, 0), noRecordsLabel); // 添加标签到 tableView
    }

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void CheckupRecord::onViewReportClicked(const QModelIndex &index)
{

}
