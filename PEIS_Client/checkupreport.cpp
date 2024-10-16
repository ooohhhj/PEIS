#include "checkupreport.h"
#include "ui_checkupreport.h"

CheckupReport::CheckupReport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CheckupReport)
{
    ui->setupUi(this);

    connect(ClientSocket::instance(),&ClientSocket::OnHealthExaminationRePortListResponce,this,&CheckupReport::OnHealthExaminationRePortListResponce);
}

CheckupReport::~CheckupReport()
{
    delete ui;
}

void CheckupReport::setUsername(const QString &username)
{
    this->m_username =username;
}

void CheckupReport::OnHealthExaminationRePortListResponce(const QJsonArray &reportsListArray)
{
    ui->tableView->show();

    // 清空当前模型
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"套餐名称", "日期", "预约状态", "操作"});

    // 设置表头字体
    QFont headerFont = ui->tableView->horizontalHeader()->font();
    headerFont.setPointSize(16); // 设置字体大小
    ui->tableView->horizontalHeader()->setFont(headerFont);

    // 添加数据到模型
    for (const QJsonValue &value : reportsListArray)
    {
        QJsonObject appointment = value.toObject();

        QString packageName = appointment["health_package"].toString();
        QString appointmentDate = appointment["appointment_date"].toString();
        QString status = appointment["appointment_status"].toString();

        qDebug()<<"status="<<status;

        // 仅在状态为 "已完成" 时添加到模型
        if (status == "已完成") {
            QList<QStandardItem *> rowItems;
            rowItems << new QStandardItem(packageName)
                     << new QStandardItem(appointmentDate)
                     << new QStandardItem(status)
                     << new QStandardItem();

            model->appendRow(rowItems);
        }
    }

    if (model->rowCount() > 0)
    {
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
                        "   background-color: #E0F2F7;" // 调整为淡蓝色，与背景更协调
                        "   color: #004D40;"
                        "   border: 2px solid #B0C4DE;" // 边框颜色与背景边框颜色一致
                        "   padding: 8px 16px;"
                        "   font-size: 16px;"
                        "   font-weight: bold;"
                        "   border-radius: 12px;"
                        "}"
                        "QPushButton:hover {"
                        "   background-color: #CFE5EB;" // 鼠标悬停时稍微深一点的蓝色
                        "   border: 2px solid #A0B4C8;" // 略深的边框颜色
                        "}"
                        "QPushButton:pressed {"
                        "   background-color: #A0C1D1;" // 鼠标按下时更深的蓝色
                        "   border: 2px solid #8FA7B8;" // 鼠标按下时的边框颜色
                        "}"
                        );

            // 将按钮设置到表格中
            ui->tableView->setIndexWidget(model->index(row, 3), button);

            ui->widget->setStyleSheet("background-color: transparent;"
                                      " border: none;");

            ui->tableView->setStyleSheet("background-color: #F0F8FF;"
                                         "border: 2px solid #B0C4DE;"
                                         "border-radius: 13px;");

            // 连接按钮点击信号到槽函数
            connect(button, &QPushButton::clicked, this, [this, model, row]() {
                QModelIndex index = model->index(row, 0);
                onViewReportClicked(index);
            });
        }
    }
    else {

        // 清除 placeholderLayout 中现有的组件
        QLayoutItem* item;
        while ((item = ui->placeholderLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }

        // 如果没有记录，添加占位符标签到父布局或设置到 tableView 的父容器中
        QLabel *noRecordsLabel = new QLabel("暂时没有体检记录");
        noRecordsLabel->setAlignment(Qt::AlignCenter);
        noRecordsLabel->setStyleSheet("font-size: 30px; color: #808080;"
                                      " background-color: transparent;"
                                      " border: none;"); // 设置标签样式

        ui->tableView->setStyleSheet("background-color: transparent;"
                                     " border: none;");

        // 隐藏 tableView 并显示标签
        ui->tableView->hide();
        ui->placeholderLayout->addWidget(noRecordsLabel); // 假设有一个占位符布局
    }

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void CheckupReport::onViewReportClicked(const QModelIndex &index)
{
    QString packageName = ui->tableView->model()->data(ui->tableView->model()->index(index.row(), 0)).toString();

    QString appointmentDate =ui->tableView->model()->data(ui->tableView->model()->index(index.row(),1)).toString();


    qDebug() << "编辑报告按钮被点击，患者姓名：" << m_username;

    QJsonObject obj;

    obj["patientName"]=m_username;
    obj["packageName"]=packageName;
    obj["appointmentDate"]=appointmentDate;

    emit LookCheckupreport();

    Packet packet =Protocol::createPacket(GetHealthExaminationRePortRequest,obj);

    QByteArray array =Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(array);

}

void CheckupReport::on_returnBtn_clicked()
{
    emit exitButtonClicked();
}

