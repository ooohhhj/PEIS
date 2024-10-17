#include "cancel_appointmentmanger.h"
#include "ui_cancel_appointmentmanger.h"

Cancel_AppointmentManger::Cancel_AppointmentManger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Cancel_AppointmentManger)
{
    ui->setupUi(this);

    connect(ClientSocket::instance(),&ClientSocket::OnPatientInfoResponce_Manager,this,&Cancel_AppointmentManger::showAppointment);
}

Cancel_AppointmentManger::~Cancel_AppointmentManger()
{
    delete ui;
}

void Cancel_AppointmentManger::onViewReportClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;  // 如果索引无效，则返回
    }
    QString patientName = index.sibling(index.row(), 0).data().toString();
    QString patientGender = index.sibling(index.row(), 1).data().toString();
    QString patientPhone =index.sibling(index.row(), 2).data().toString();
    QString patientBirthDate = index.sibling(index.row(), 3).data().toString();
    QString healthPackage = index.sibling(index.row(), 4).data().toString();
    QString appointmentDate = index.sibling(index.row(), 5).data().toString();
    QString appointmentStatus = index.sibling(index.row(), 6).data().toString();


    if (appointmentStatus == "已预约") {
        AppointmentManager(patientName,healthPackage,appointmentDate,appointmentStatus);

    } else if (appointmentStatus == "已完成")
    {
        emit onLookCheckUpReportClicked(patientName,healthPackage,appointmentDate);
    }
}

void Cancel_AppointmentManger::AppointmentManager(const QString &patientName, const QString &healthPackage,const QString &appointmentDate, const QString &appointmentStatus)
{
    // 创建对话框
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("预约信息");
    dialog->setWindowFlags(dialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);  // 移除帮助按钮

    // 设置对话框的大小
    dialog->setFixedSize(500, 400);

    // 设置对话框图标
    QIcon dialogIcon(":/appointment.png");  // 替换为你的图标路径
    dialog->setWindowIcon(dialogIcon);

    // 创建控件
    QLabel *nameLabel = new QLabel("姓名: " + patientName, dialog);
    QLabel *packageLabel = new QLabel("套餐: " + healthPackage, dialog);
    QLabel *dateLabel = new QLabel("预约时间: " + appointmentDate, dialog);
    QLabel *statusLabel = new QLabel("预约状态: " + appointmentStatus, dialog);

    QPushButton *cancelButton = new QPushButton("取消", dialog);

    QString labelStyle = "background-color: transparent; border: none; font-size: 12pt;";  // 增加字体大小

    nameLabel->setStyleSheet(labelStyle);
    packageLabel->setStyleSheet(labelStyle);
    dateLabel->setStyleSheet(labelStyle);
    statusLabel->setStyleSheet(labelStyle);

    cancelButton->setStyleSheet("font-size: 12pt;");


    // 设置布局
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(nameLabel);
    layout->addWidget(packageLabel);
    layout->addWidget(dateLabel);
    layout->addWidget(statusLabel);
    layout->addWidget(cancelButton);

    dialog->setLayout(layout);

    // 连接信号与槽
    connect(cancelButton, &QPushButton::clicked, this,[this, dialog,patientName, healthPackage, appointmentDate]() {
        dialog->close();
        OncancelAppointmentButton(patientName, healthPackage, appointmentDate);

    });
    // 以模态方式显示对话框
    dialog->exec();

    // 释放对话框内存
    delete dialog;
}

void Cancel_AppointmentManger::OncancelAppointmentButton(const QString &username, const QString &packageName, const QString &appointmentDate)
{
    qDebug()<<"取消按钮被点击";

    QJsonObject obj;
    obj["username"] =username;
    obj["packageName"]=packageName;
    obj["appointmentDate"] =appointmentDate;

    Packet packet =Protocol::createPacket(CancelAppointmentRequest,obj);

    QByteArray array  = Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(array);
}

void Cancel_AppointmentManger::setPatientName(const QString &patientName)
{
    this->m_patientName = patientName;
}

void Cancel_AppointmentManger::showAppointment(const QJsonArray &patientInfoArray)
{
    // 清空当前模型
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"患者姓名", "性别","联系方式","出生日期","套餐名称","预约日期","预约状态","操作"});

    // 设置表头字体
    QFont headerFont = ui->tableView->horizontalHeader()->font();
    headerFont.setPointSize(16); // 设置字体大小
    ui->tableView->horizontalHeader()->setFont(headerFont);


    // 添加数据到模型
    for (const QJsonValue &value : patientInfoArray)
    {
        QJsonObject appointment = value.toObject();

        QString patientName = appointment["patientName"].toString();
        QString patientGender = appointment["patientGender"].toString();
        QString patientPhone =appointment["patientPhone"].toString();
        QString patientBirthDate = ClientSocket::instance()->convertAppointmentDate(appointment["patientBirthDate"].toString());
        QString healthPackage = appointment["healthPackage"].toString();
        QString appointmentDate = ClientSocket::instance()->convertAppointmentDate(appointment["appointmentDate"].toString());

        QString appointmentStatus = appointment["appointmentStatus"].toString();


        QList<QStandardItem *> rowItems;
        rowItems << new QStandardItem(patientName)
                 << new QStandardItem(patientGender)
                 << new QStandardItem(patientPhone)
                 << new QStandardItem(patientBirthDate)
                 <<new QStandardItem(healthPackage)
                <<new QStandardItem(appointmentDate)
               <<new QStandardItem(appointmentStatus)
              << new QStandardItem();


        model->appendRow(rowItems);
    }

    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // 设置为不可编辑


    // 添加按钮到 "操作" 列
    for (int row = 0; row < model->rowCount(); ++row) {
        QString status = model->item(row, 6)->text();  // 获取预约状态

        QPushButton *button = new QPushButton();
        if (status == "已预约" ) {
            button->setText("取消预约");
        } else if (status == "已体检" ||status == "已完成") {
            button->setText("查看报告");
        } else {
            button->setText("未知操作");
        }

        button->setStyleSheet(
                    "QPushButton {"
                    "   background-color: #A2D8D0;"      // Lighter Sea Green 背景色，与边框相配
                    "   color: #004D40;"                 // 深绿色文字
                    "   border: 2px solid #78C1B1;"      // 边框为稍深的绿色
                    "   padding: 8px 16px;"              // 内边距增加，按钮更大
                    "   font-size: 16px;"                // 字体稍大，视觉舒适
                    "   font-weight: bold;"              // 字体加粗
                    "   border-radius: 12px;"            // 圆角，按钮更柔和
                    "}"
                    "QPushButton:hover {"
                    "   background-color: #87C5BB;"      // 鼠标悬停时的颜色，稍深一点的绿色
                    "   border: 2px solid #5DA49D;"      // 鼠标悬停时边框色稍深
                    "}"
                    "QPushButton:pressed {"
                    "   background-color: #5DA49D;"      // 按下时的背景色，深绿色
                    "   border: 2px solid #468882;"      // 按下时的边框色
                    "}"
                    );


        // 将按钮设置到表格中
        ui->tableView->setIndexWidget(model->index(row, 7), button);  // 使用 setIndexWidget 设置按钮到 "操作" 列

        // 连接按钮点击信号到槽函数
        connect(button, &QPushButton::clicked, this, [this,model, row]() {
            QModelIndex index = model->index(row, 0);  // 获取第 row 行的 QModelIndex
            onViewReportClicked(index);  // 传递 QModelIndex 而不是 int // 假设你有一个槽函数处理报告点击事件
        });
    }

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


void Cancel_AppointmentManger::on_refreshButton_clicked()
{
    QJsonObject obj ;
    obj["patientName"] =m_patientName;

    qDebug()<<"patientName="<<m_patientName;

    Packet packet = Protocol::createPacket(PatientInfoRequest_Nurse,obj);

    QByteArray array =Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(array);
}

