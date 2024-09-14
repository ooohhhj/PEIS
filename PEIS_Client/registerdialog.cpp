#include "registerdialog.h"
#include "ui_registerdialog.h"



RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);

    setBirthDate();

}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::setBirthDate()
{

    //获取当前的年月日
     int currentYear=QDate::currentDate().year();
     int currentMonth =QDate::currentDate().month();
     int currentDay = QDate::currentDate().day();

     //初始化年月日范围为当前年月份范围
      //设置年份范围
     for(int i=1900;i<=currentYear;++i)
     {
         ui->yearComboBox->addItem(QString::number(i));
     }

     //设置月份
     for(int i=1;i<=currentMonth;i++)
     {
         ui->monthComboBox->addItem(QString::number(i));
     }

     //设置天数
     for(int i=1;i<=currentDay;i++)
     {
         ui->dayComboBox->addItem(QString::number(i));
     }

     //设置当前默认日期为今天
     ui->yearComboBox->setCurrentText(QString::number(currentYear));
     ui->monthComboBox->setCurrentText(QString::number(currentMonth));
     ui->dayComboBox->setCurrentText(QString::number(currentDay));


     //监听年份或月份的变化 并动态更新月份选项或日期选项
     connect(ui->yearComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onYearChanged(int)));
     connect(ui->monthComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onMonthChanged(int)));

}

void RegisterDialog::onYearChanged(int)
{
    int selectedYear= ui->yearComboBox->currentText().toInt();
    int currentYear = QDate::currentDate().year();
    int currentMonth = QDate::currentDate().month();

    ui->monthComboBox->clear();
    if(selectedYear == currentYear) //选择的年份与当前年份相同
    {
        for(int i=1;i<=currentMonth;i++)
        {
            ui->monthComboBox->addItem(QString::number(i));
        }
        return;
    }
    else
    {
        for(int i=1;i<=12;i++)
        {
            ui->monthComboBox->addItem(QString::number(i));
        }
    }
}

void RegisterDialog::onMonthChanged(int)
{
    QComboBox * yearComboBox = findChild<QComboBox*>("yearComboBox");
    QComboBox * monthComboBox  =findChild<QComboBox*>("monthComboBox");
    updateDaysComboBox(yearComboBox,monthComboBox);
}

void RegisterDialog::updateDaysComboBox(QComboBox *yearComboBox, QComboBox *monthComboBox)
{
    int year = yearComboBox->currentText().toInt();
    int month = monthComboBox->currentText().toInt();

    //获取天数
    int daysInMonth = QDate(year,month,1).daysInMonth();

    //清楚旧的日期并重新填充新的日期选项
    ui->dayComboBox->clear();

    for(int i=1;i<=daysInMonth;i++)
    {
        ui->dayComboBox->addItem(QString::number(i));
    }
}


