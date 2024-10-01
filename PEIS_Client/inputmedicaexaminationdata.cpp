#include "inputmedicaexaminationdata.h"
#include "ui_inputmedicaexaminationdata.h"

InputMedicaExaminationData::InputMedicaExaminationData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputMedicaExaminationData)
{
    ui->setupUi(this);

    connect(ClientSocket::instance(),&ClientSocket::OnHealthCheckupItemResponce,this,&InputMedicaExaminationData::onHealthCheckupItemResponce);

    connect(ClientSocket::instance(),&ClientSocket::OnRecordHealthCheckupResponce,this,&InputMedicaExaminationData::on_OnRecordHealthCheckupResponce);
}

InputMedicaExaminationData::~InputMedicaExaminationData()
{
    delete ui;
}

void InputMedicaExaminationData::setPatientInfo(const QString &patientName, const QString &patientGender, const QString &patientPhone, const QString &patientBirthDate, const QString &healthPackage, const QString &appointmentDate, const QString &appointmentStatus)
{
    ui->patientName->setText(patientName);
    ui->gender->setText(patientGender);
    ui->birthDate->setText(patientBirthDate);
    ui->phoneNumber->setText(patientPhone);
    ui->packageName->setText(healthPackage);
    ui->packageDate->setText(appointmentDate);
    ui->status->setText(appointmentStatus);
}

void InputMedicaExaminationData::onHealthCheckupItemResponce(const QJsonArray &packageItems)
{
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(ui->packageItemsWidget);

    // 创建一个 QScrollArea 用于显示动态内容
    QScrollArea *scrollArea = new QScrollArea(ui->packageItemsWidget);
    scrollArea->setStyleSheet("background-color: transparent; border: none;");
    QWidget *scrollWidget = new QWidget(ui->packageItemsWidget);
    QGridLayout *gridLayout = new QGridLayout(scrollWidget);


    // 使用循环添加每一项
    for (int i = 0; i < packageItems.size(); ++i)
    {
        QJsonObject item = packageItems[i].toObject();
        QString itemName = item["itemName"].toString();
        QString normalRange = item["normalRange"].toString();

        // 项目名称
        QLabel *itemNameLabel = new QLabel(itemName, ui->packageItemsWidget);
        itemNameLabel->setObjectName(QString("itemNameLabel_%1").arg(i));  // 设置唯一的 objectName
        itemNameLabel->setStyleSheet("background-color: transparent; border: none; font-size: 14pt; font-weight: bold;");
        gridLayout->addWidget(itemNameLabel, i + 1, 0);

        // 项目数据输入框
        QLineEdit *inputField = new QLineEdit(ui->packageItemsWidget);
        inputField->setObjectName(QString("inputField_%1").arg(i));  // 设置唯一的 objectName
        inputField->setStyleSheet("background-color: transparent; border:  2px solid black; font-size: 12pt;");
        gridLayout->addWidget(inputField, i + 1, 1);

        // 正常数据范围
        QLabel *normalRangeLabel = new QLabel(normalRange, ui->packageItemsWidget);
        normalRangeLabel->setObjectName(QString("normalRangeLabel_%1").arg(i));  // 设置唯一的 objectName
        normalRangeLabel->setStyleSheet("background-color: transparent; border: none; font-size: 14pt; font-weight: bold;");
        gridLayout->addWidget(normalRangeLabel, i + 1, 2);

        // 体检结果输入框
        QLineEdit *resultField = new QLineEdit(ui->packageItemsWidget);
        resultField->setObjectName(QString("resultField%1").arg(i));  // 设置唯一的 objectName
        resultField->setStyleSheet("background-color: transparent; border:  2px solid black; font-size: 12pt;");  // 更换背景色
        gridLayout->addWidget(resultField, i + 1, 3);

        // 项目负责人输入框
        QLineEdit *responsiblePersonField = new QLineEdit(ui->packageItemsWidget);
        responsiblePersonField->setObjectName(QString("responsiblePersonField_%1").arg(i));  // 设置唯一的 objectName
        responsiblePersonField->setStyleSheet("background-color: transparent; border:  2px solid black; font-size: 12pt;");
        gridLayout->addWidget(responsiblePersonField, i + 1, 4);
    }

    // 将 gridLayout 设置为 scrollWidget 的布局
    scrollWidget->setLayout(gridLayout);

    // 将 scrollWidget 设置为 scrollArea 的子控件
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);

    // 将 scrollArea 添加到主布局中
    mainLayout->addWidget(scrollArea);

    // 设置主布局给 packageItemsWidget
    ui->packageItemsWidget->setLayout(mainLayout);

}

void InputMedicaExaminationData::on_submitButton_clicked()
{
    //获取姓名
    QString patientName =ui->patientName->text();

    //获取性别
    QString gender =ui->gender->text();

    //获取出生日期
    QString birthDate =ui->birthDate->text();

    //获取手机号码
    QString phoneNumber =ui->phoneNumber->text();

    //获取套餐名称
    QString packageName =ui->packageName->text();

    //获取体检时间
    QString packageDate =ui->packageDate->text();

    // 获取包裹项的数量
    int itemCount = ui->packageItemsWidget->findChildren<QLineEdit *>().size() / 4; // 假设每个体检项目有4个输入框

    QJsonArray HealthCheckupDate;

    QString status = "待审核";

    QString str =status;

    // 遍历所有输入框
    for (int i = 0; i < itemCount; ++i)
    {
        QLabel *itemNameLabel = ui->packageItemsWidget->findChild<QLabel *>(QString("itemNameLabel_%1").arg(i));
        QLineEdit *inputField = ui->packageItemsWidget->findChild<QLineEdit *>(QString("inputField_%1").arg(i));
        QLabel *normalRangeLabel = ui->packageItemsWidget->findChild<QLabel *>(QString("normalRangeLabel_%1").arg(i));
        QLineEdit *resultField =ui->packageItemsWidget->findChild<QLineEdit*>(QString("resultField%1").arg(i));
        QLineEdit *responsiblePersonField = ui->packageItemsWidget->findChild<QLineEdit *>(QString("responsiblePersonField_%1").arg(i));

        if (itemNameLabel && inputField && normalRangeLabel && responsiblePersonField)
        {
            // 读取数据并打包成 JSON 对象
            QJsonObject itemData;
            itemData["itemName"] = itemNameLabel->text();
            itemData["inputData"] = inputField->text();
            itemData["normalRange"] = normalRangeLabel->text();
            itemData["resultField"] =resultField->text();
            itemData["responsiblePerson"] = responsiblePersonField->text();


            if(inputField->text().isEmpty())
            {
                str="体检中";
            }
            qDebug()<<itemNameLabel->text()<<":"<<inputField->text()<<"/"<<normalRangeLabel->text()<<" "<<responsiblePersonField->text();

            // 将该项目添加到提交数组中
            HealthCheckupDate.append(itemData);
        }
    }

    if(str!= status)
    {
        status =str;
    }

    QJsonObject Date;
    Date["patientName"] =patientName;
    Date["gender"] = gender;
    Date["birthDate"]=birthDate;
    Date["phoneNumber"] =phoneNumber;
    Date["packageName"]=packageName;
    Date["packageDate"]=packageDate;

    Date["HealthCheckupDate"] =HealthCheckupDate;
    Date["status"] =status;

    Packet packet =Protocol::createPacket(HealthCheckupDataRequest,Date);

    QByteArray array =Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(array);

}

void InputMedicaExaminationData::on_OnRecordHealthCheckupResponce(const QJsonArray &checkupDate)
{

    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(ui->packageItemsWidget);

    // 创建一个 QScrollArea 用于显示动态内容
    QScrollArea *scrollArea = new QScrollArea(ui->packageItemsWidget);
    scrollArea->setStyleSheet("background-color: transparent; border: none;");

    QWidget *scrollWidget = new QWidget(ui->packageItemsWidget);
    QGridLayout *gridLayout = new QGridLayout(scrollWidget);

    // 使用循环添加每一项
    for (int i = 0; i < checkupDate.size(); ++i)
    {
        QJsonObject item = checkupDate[i].toObject();
        QString itemName = item["item_name"].toString();
        QString normalRange = item["normal_range"].toString();
        QString inputData = item["input_data"].toString(); // 从 QJsonObject 获取输入数据
        QString resultDate = item["result_date"].toString();
        QString responsiblePerson = item["responsible_person"].toString(); // 从 QJsonObject 获取负责人数据

        // 项目名称
        QLabel *itemNameLabel = new QLabel(itemName, ui->packageItemsWidget);
        itemNameLabel->setObjectName(QString("itemNameLabel_%1").arg(i));  // 设置唯一的 objectName
        itemNameLabel->setStyleSheet("background-color: transparent; border: none; font-size: 14pt; font-weight: bold;");
        gridLayout->addWidget(itemNameLabel, i + 1, 0);

        // 项目数据输入框
        QLineEdit *inputField = new QLineEdit(ui->packageItemsWidget);
        inputField->setObjectName(QString("inputField_%1").arg(i));  // 设置唯一的 objectName
        inputField->setStyleSheet("background-color: transparent; border:  2px solid black; font-size: 12pt;");
        inputField->setText(inputData); // 设置从数据库查询到的输入数据

        // 判断输入框是否有数据，有数据则设置为只读
        if (!inputData.isEmpty()) {
            inputField->setReadOnly(true); // 设置为只读
        }
        gridLayout->addWidget(inputField, i + 1, 1);

        // 正常数据范围
        QLabel *normalRangeLabel = new QLabel(normalRange, ui->packageItemsWidget);
        normalRangeLabel->setObjectName(QString("normalRangeLabel_%1").arg(i));  // 设置唯一的 objectName
        normalRangeLabel->setStyleSheet("background-color: transparent; border: none; font-size: 14pt; font-weight: bold;");
        gridLayout->addWidget(normalRangeLabel, i + 1, 2);

        // 体检结果输入框
        QLineEdit *resultField = new QLineEdit(ui->packageItemsWidget);
        resultField->setObjectName(QString("resultField%1").arg(i));  // 设置唯一的 objectName
        resultField->setStyleSheet("background-color: transparent; border:  2px solid black; font-size: 12pt;");  // 更换背景色
        resultField->setText(resultDate); // 设置从数据库查询到的输入数据

        // 判断输入框是否有数据，有数据则设置为只读
        if (!resultDate.isEmpty()) {
            resultField->setReadOnly(true); // 设置为只读
        }
        gridLayout->addWidget(resultField, i + 1, 3);



        // 项目负责人输入框
        QLineEdit *responsiblePersonField = new QLineEdit(ui->packageItemsWidget);
        responsiblePersonField->setObjectName(QString("responsiblePersonField_%1").arg(i));  // 设置唯一的 objectName
        responsiblePersonField->setStyleSheet("background-color: transparent; border:  2px solid black; font-size: 12pt;");
        responsiblePersonField->setText(responsiblePerson); // 设置从数据库查询到的负责人数据

        // 判断负责人输入框是否有数据，有数据则设置为只读
        if (!responsiblePerson.isEmpty()) {
            responsiblePersonField->setReadOnly(true); // 设置为只读
        }
        gridLayout->addWidget(responsiblePersonField, i + 1, 4);
    }

    // 将 gridLayout 设置为 scrollWidget 的布局
    scrollWidget->setLayout(gridLayout);

    // 将 scrollWidget 设置为 scrollArea 的子控件
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);

    // 将 scrollArea 添加到主布局中
    mainLayout->addWidget(scrollArea);

    // 设置主布局给 packageItemsWidget
    ui->packageItemsWidget->setLayout(mainLayout);
}


