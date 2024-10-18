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
    ui->tableView->show();

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
        QString status = model->item(row, 4)->text();  // 获取预约状态

        QPushButton *button = new QPushButton();
        if (status == "已体检") {
            button->setText("编辑报告");
        } else if (status == "已完成") {
            button->setText("查看报告");
        } else {
            button->setText(status);
        }

        button->setStyleSheet(
                    "QPushButton {"
                    "   background-color: #A2D8D0;"      // 背景色，Lighter Sea Green
                    "   color: #004D40;"                 // 深绿色文字
                    "   border: 2px solid #78C1B1;"      // 边框为稍深的绿色
                    "   padding: 8px 16px;"              // 内边距增加，按钮更大
                    "   font-size: 18px;"                // 字体稍大，视觉舒适
                    "   font-weight: bold;"              // 字体加粗
                    "   border-radius: 12px;"            // 圆角，按钮更柔和
                    "}"
                    "QPushButton:hover {"
                    "   background-color: #92D3C7;"      // 鼠标悬停时的颜色，稍深一点的绿色
                    "   border: 2px solid #6BAF9F;"      // 鼠标悬停时边框色稍深
                    "}"
                    "QPushButton:pressed {"
                    "   background-color: #78C1B1;"      // 按下时的背景色，深绿色
                    "   border: 2px solid #4F9E89;"      // 按下时的边框色
                    "}"
                    );



        // 将按钮设置到表格中
        ui->tableView->setIndexWidget(model->index(row, 5), button);  // 使用 setIndexWidget 设置按钮到 "操作" 列

        ui->widget->setStyleSheet("background-color: transparent;"
                                  " border: none;");

        ui->tableView->setStyleSheet("background-color: #F5FFFA; "
                                     "border: 2px solid #A2D8D0;");

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
    if (!index.isValid()) {
        return;  // 如果索引无效，则返回
    }

    QString appointmentStatus = index.sibling(index.row(), 4).data().toString();

    QString patientName = ui->tableView->model()->data(ui->tableView->model()->index(index.row(), 0)).toString();

    QString packageName = ui->tableView->model()->data(ui->tableView->model()->index(index.row(),2)).toString();

    QString appointmentDate =ui->tableView->model()->data(ui->tableView->model()->index(index.row(),3)).toString();

    if (appointmentStatus == "已体检")
    {
        qDebug() << "编辑报告按钮被点击，患者姓名：" << patientName;

        emit EditCheckuppreport(patientName,packageName,appointmentDate,2);

    } else if (appointmentStatus == "已完成")
    {
        qDebug()<<"已完成发送";
        emit onLookCheckUpReportClicked(patientName,packageName,appointmentDate,2);
    }

}


void AppointmentManagement::setUsername(const QString &username)
{
    this->m_username = username;
}

void AppointmentManagement::on_returnExitButton_clicked()
{
    emit exitButtonClicked();
}

