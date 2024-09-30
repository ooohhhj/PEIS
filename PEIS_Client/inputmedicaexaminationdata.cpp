#include "inputmedicaexaminationdata.h"
#include "ui_inputmedicaexaminationdata.h"

InputMedicaExaminationData::InputMedicaExaminationData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputMedicaExaminationData)
{
    ui->setupUi(this);

    connect(ClientSocket::instance(),&ClientSocket::OnHealthCheckupItemResponce,this,&InputMedicaExaminationData::onHealthCheckupItemResponce);
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
{// 创建主布局
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
        itemNameLabel->setStyleSheet("background-color: transparent; border: none; font-size: 14pt; font-weight: bold;");
        gridLayout->addWidget(itemNameLabel, i + 1, 0);

        // 项目数据输入框
        QLineEdit *inputField = new QLineEdit(ui->packageItemsWidget);
        inputField->setStyleSheet("background-color: transparent; border:  2px solid black; font-size: 12pt;");
        gridLayout->addWidget(inputField, i + 1, 1);

        // 正常数据范围
        QLabel *normalRangeLabel = new QLabel(normalRange, ui->packageItemsWidget);
        gridLayout->addWidget(normalRangeLabel, i + 1, 2);
        normalRangeLabel->setStyleSheet("background-color: transparent; border: none; font-size: 14pt; font-weight: bold;");

        // 体检结果输入框
        QLineEdit *resultField = new QLineEdit(ui->packageItemsWidget);
        resultField->setStyleSheet("background-color: transparent; border:  2px solid black; font-size: 12pt;");
        gridLayout->addWidget(resultField, i + 1, 3);

        // 项目负责人输入框
        QLineEdit *responsiblePersonField = new QLineEdit(ui->packageItemsWidget);
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
    qDebug() << "提交按钮被点击，处理数据";
}

