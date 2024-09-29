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
                 <<new QStandardItem(status);

        model->appendRow(rowItems);
    }

    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // 设置为不可编辑

    // 设置委托用于最后一列（操作列）
    Delegate *delegate = new Delegate(this);
    ui->tableView->setItemDelegateForColumn(5, delegate);

    // 连接按钮点击信号到槽函数
    connect(delegate, &Delegate::buttonClicked, this, &AppointmentManagement::onViewReportClicked);

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

