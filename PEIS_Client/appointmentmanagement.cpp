#include "appointmentmanagement.h"
#include "ui_appointmentmanagement.h"

AppointmentManagement::AppointmentManagement(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppointmentManagement)
{
    ui->setupUi(this);


    connect(ClientSocket::instance(),&ClientSocket::OnstartDate,this,&AppointmentManagement::setDateQComBOx);
    connect(ClientSocket::instance(),&ClientSocket::OnAppointmentsDate,this,&AppointmentManagement::OnAppointmentsDate);

}

AppointmentManagement::~AppointmentManagement()
{
    delete ui;
}


void AppointmentManagement::setDateQComBOx(const QString &startDate)
{
    qDebug() << "startDate:" << startDate; // 检查 startDate 的值
    QString datePart = startDate.split("T")[0]; // 获取日期部分
    QDate date = QDate::fromString(datePart, "yyyy-MM-dd");

    if (!date.isValid()) {
        qDebug() << "Invalid date format!";
        return; // 处理无效日期
    }

    QDate currentDate = QDate::currentDate();
    qDebug() << "currentDate=" << currentDate;

    while (date <= currentDate)
    {
        ui->startDateCBox->addItem(date.toString("yyyy-MM"), startDate);
        ui->endDateCBox->addItem(date.toString("yyyy-MM"), startDate);
        date = date.addMonths(1);
        qDebug() << "date=" << date;
    }
}

void AppointmentManagement::OnAppointmentsDate(const QJsonArray &appointments)
{
    // 清空当前模型
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"患者姓名", "联系方式","套餐名称","预约日期",  "预约状态","操作"});

    // 设置表头字体
    QFont headerFont = ui->tableView->horizontalHeader()->font();
    headerFont.setPointSize(16); // 设置字体大小
    ui->tableView->horizontalHeader()->setFont(headerFont);


    // 添加数据到模型
    for (const QJsonValue &value : appointments) {
        QJsonObject appointment = value.toObject();

        QString patientName = appointment["patient_name"].toString();
        QString contactNumber = appointment["patient_phone"].toString();
        QString packageName =appointment["health_package"].toString();
        QString appointmentDate = appointment["appointment_date"].toString();
        QString status = appointment["appointment_status"].toString();


        QList<QStandardItem *> rowItems;
        rowItems << new QStandardItem(patientName)
                 << new QStandardItem(contactNumber)
                 << new QStandardItem(packageName)
                 << new QStandardItem(appointmentDate)
                 <<new QStandardItem(status)
                << new QStandardItem();


        model->appendRow(rowItems);
    }

    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // 设置为不可编辑

    // 添加按钮到 "操作" 列
    for (int row = 0; row < model->rowCount(); ++row) {
        QString status = model->item(row, 5)->text();  // 获取预约状态

        QPushButton *button = new QPushButton();
        if (status == "已体检") {
            button->setText("编辑报告");
        } else if (status == "已完成") {
            button->setText("查看报告");
        } else {
            button->setText("未知操作");
        }

        // 设置按钮样式
        button->setStyleSheet(
                    "QPushButton {"
                    "   background-color: #4CAF50;"
                    "   color: white;"
                    "   border: none;"
                    "   padding: 5px 10px;"
                    "   font-size: 14px;"
                    "   border-radius: 10px;"
                    "}"
                    "QPushButton:hover {"
                    "   background-color: #45a049;"
                    "}"
                    );

        // 将按钮设置到表格中
        ui->tableView->setIndexWidget(model->index(row, 5), button);  // 使用 setIndexWidget 设置按钮到 "操作" 列

        // 连接按钮点击信号到槽函数
        connect(button, &QPushButton::clicked, this, [this,model, row]() {
            QModelIndex index = model->index(row, 0);  // 获取第 row 行的 QModelIndex
            onViewReportClicked(index);  // 传递 QModelIndex 而不是 int // 假设你有一个槽函数处理报告点击事件
        });
    }

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void AppointmentManagement::onViewReportClicked(const QModelIndex &index)
{
    QString patientName = ui->tableView->model()->data(ui->tableView->model()->index(index.row(), 0)).toString();

    qDebug() << "查看报告按钮被点击，患者姓名：" << patientName;

}


void AppointmentManagement::setUsername(const QString &username)
{
    this->m_username = username;
}

