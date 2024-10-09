#include "editmedicalexaminationreport.h"
#include "ui_editmedicalexaminationreport.h"

EditMedicalExaminationReport::EditMedicalExaminationReport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditMedicalExaminationReport)
{
    ui->setupUi(this);

    connect(ClientSocket::instance(),&ClientSocket::OnPatientHealthExaminationDateResponce,this,&EditMedicalExaminationReport::OnPatientHealthExaminationDateResponce);

}

EditMedicalExaminationReport::~EditMedicalExaminationReport()
{
    delete ui;
}

void EditMedicalExaminationReport::setCheckupDate(const QJsonArray &checkupDate)
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

void EditMedicalExaminationReport::OnPatientHealthExaminationDateResponce(const QJsonObject &patientDateArray)
{
    QString patientName = patientDateArray["patientName"].toString();
    QString gender = patientDateArray["gender"].toString();
    QString birth_date = patientDateArray["birth_date"].toString();
    QString phone_number = patientDateArray["phone_number"].toString();
    QString package_name = patientDateArray["package_name"].toString();
    QString package_date = patientDateArray["package_date"].toString();
    QString report_generated =patientDateArray["report_generated"].toString();
    QString report_status =patientDateArray["report_status"].toString();
    QString doctor_name =patientDateArray["doctor_name"].toString();
    QString doctor_advice =patientDateArray["doctor_advice"].toString();

    QJsonArray checkupDate =patientDateArray["CheckupDate"].toArray();

    ui->patientName->setText(patientName);
    ui->gender->setText(gender);
    ui->birthDate->setText(birth_date);
    ui->phoneNumber->setText(phone_number);
    ui->packageName->setText(package_name);
    ui->packageDate->setText(package_date);
    ui->report_generateTime->setText(report_generated);
    ui->status->setText(report_status);


    setCheckupDate(checkupDate);


}

void EditMedicalExaminationReport::on_submitButton_clicked()
{
    QString doctorAdvice =ui->doctorAdvice->toPlainText();
    QString editDorctor = ui->EditDorctor->text();

    if(doctorAdvice.isEmpty() || editDorctor.isEmpty())
    {
        ClientSocket::instance()->showMessageBox(":/warning.png","预约管理","输入信息为空！！");
        return ;
    }


    //报告生成时间
    QString reportGenerateTime = QDate::currentDate().toString("yyyy-MM-dd");


    QString patientName =ui->patientName->text();
    QString packageName =ui->packageName->text();
    QString packageDate =ui->packageDate->text();

    qDebug()<<"patientName="<<patientName;
    qDebug()<<"packageName="<<packageName;
    qDebug()<<"packageDate="<<packageDate;

    QJsonObject obj;

    obj["patientName"]=patientName;
    obj["packageName"]=packageName;
    obj["packageDate"]=packageDate;
    obj["doctorAdvice"]=doctorAdvice;
    obj["editDorctor"]=editDorctor;
    obj["reportGenerateTime"]=reportGenerateTime;

    Packet packet =Protocol::createPacket(SaveReportRequest,obj);

    QByteArray array =Protocol::serializePacket(packet);

    ClientSocket::instance()->senData(array);
}

