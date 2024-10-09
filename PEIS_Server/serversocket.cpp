#include "serversocket.h"
#include "ui_serversocket.h"

ServerSocket::ServerSocket(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ServerSocket)
{
    ui->setupUi(this);

    tcpServer = new QTcpServer(this);
    threadPool = new QThreadPool(this);

    //设置线程池中最大线程数
    threadPool->setMaxThreadCount(100);

    //启动服务端
    if(!tcpServer->listen(QHostAddress::Any,9600))
    {
        qDebug()<<"Server could not start!";
    }
    else
    {
        qDebug()<<"Server started";
        connect(tcpServer,&QTcpServer::newConnection,this,&ServerSocket::newConnection);
    }
}

ServerSocket::~ServerSocket()
{
    delete ui;
}

QString ServerSocket::generateHealthCheckupReport(const QSqlQuery &query, const QJsonArray &dateArray)
{
    QString patient_Name = query.value("patient_name").toString();
    QString gender = query.value("gender").toString();
    QString birth_date = query.value("birth_date").toString();
    QString phone_number = query.value("phone_number").toString();
    QString doctor_name = query.value("doctor_name").toString();
    QString package_name = query.value("package_name").toString();
    QString package_date = query.value("package_date").toString();
    QString report_generated = QDate::currentDate().toString("yyyy-MM-dd"); // 当前生成时间
    QString doctor_advice = query.value("doctor_advice").toString();

    // 生成报告文件路径
    QString reportPath = QString("H:/PEIS/PEIS/reports/%1_%2_%3_report.pdf")
            .arg(patient_Name)
            .arg(package_name)
            .arg(QDate::currentDate().toString("yyyyMMdd"));

    // 创建打印机对象
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(reportPath);
    printer.setPageSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Portrait);

    // 创建绘图工具
    QPainter painter(&printer);
    painter.setFont(QFont("Arial", 12));

    // 绘制标题
    painter.drawText(100, 50, "体检报告");

    // 绘制基本信息
    painter.drawText(100, 100, "患者姓名: " + patient_Name);
    painter.drawText(100, 130, "性别: " + gender);
    painter.drawText(100, 160, "出生日期: " + birth_date);
    painter.drawText(100, 190, "电话: " + phone_number);
    painter.drawText(100, 220, "套餐名称: " + package_name);
    painter.drawText(100, 250, "套餐日期: " + package_date);
    painter.drawText(100, 280, "报告生成时间: " + report_generated);

    // 绘制体检项目结果
    painter.drawText(100, 320, "体检项目结果:");

    int yOffset = 350; // Y坐标偏移量
    for (const QJsonValue &value : dateArray)
    {
        QJsonObject dateObject = value.toObject();
        QString itemName = dateObject["item_name"].toString();
        QString normalRange = dateObject["normal_range"].toString();
        QString inputData = dateObject["input_data"].toString();
        QString resultDate = dateObject["result_date"].toString();
        QString responsiblePerson = dateObject["responsible_person"].toString();

        painter.drawText(100, yOffset, QString("%1: %2 (正常范围: %3, 结果日期: %4, 责任人: %5)")
                         .arg(itemName, inputData, normalRange, resultDate, responsiblePerson));
        yOffset += 30; // 每行增加30个单位的偏移量
    }

    // 绘制医生姓名和建议
    painter.drawText(100, yOffset, "医生姓名: " + doctor_name);
    painter.drawText(100, yOffset + 30, "医生建议: " + doctor_advice);

    // 结束绘图
    painter.end();

    // 返回生成的报告路径
    return reportPath;
}

void ServerSocket::newConnection()
{
    qDebug()<<"有新的客户端";
    while(tcpServer->hasPendingConnections())
    {
        QTcpSocket *clientSocket = tcpServer->nextPendingConnection();

        //当有数据可读时处理
        connect(clientSocket,&QTcpSocket::readyRead,this,[this,clientSocket]()
        {
            //创建工作对象
            ClientHandler * handler =new ClientHandler(clientSocket);

            //接收线程处理后的数据 发送给客户端
            connect(handler,&ClientHandler::requestProcessed,this,[this,clientSocket](const QByteArray & response)
            {
                //将处理结果发送给客户端
                clientSocket->write(response);
            });

            connect(handler,&ClientHandler::InsertDoctorMapById,this,&ServerSocket::InsertDoctorMapById);

            connect(handler,&ClientHandler::ForwardMessage,this,&ServerSocket::ForwardMessage);

            connect(handler,&ClientHandler::generateReport,this,&ServerSocket::onGenerateReport);

            //将数据交给线程池中的任务处理
            threadPool->start(handler);
        });

        //监听客户端断开连接
        connect(clientSocket,&QTcpSocket::disconnected,clientSocket,&QTcpSocket::deleteLater);

    }
}

void ServerSocket::InsertDoctorMapById(const int &doctorId, QTcpSocket *clientSocket)
{
    // 加锁
    QWriteLocker  locker(&rwLock);

    if (!doctorConnections.contains(doctorId)) {
        doctorConnections.insert(doctorId, clientSocket);
        qDebug() << "Inserted doctorId=" << doctorId << " into doctorConnections";
    } else {
        qDebug() << "doctorId=" << doctorId << " already exists in doctorConnections";
    }
}

void ServerSocket::ForwardMessage(const int &doctorId)
{
    QReadLocker locker(&rwLock);
    if (doctorConnections.contains(doctorId))
    {
        QTcpSocket *doctorSocket = doctorConnections[doctorId];
        //发送
        QJsonObject obj;

        Packet packet =Protocol::createPacket(PatientHealthExaminationReview,obj);

        QByteArray array =Protocol::serializePacket(packet);

        doctorSocket->write(array);
    }
}



void ServerSocket::onGenerateReport(QTcpSocket *socket,const QSqlQuery &query, const QJsonArray &dateArray, const QString &patientName, const QString &packageName, const QString &packageDate)
{
    QJsonObject obj;
    QString reportPath = generateHealthCheckupReport(query, dateArray);

    // 检查报告生成是否成功
    if(reportPath.isEmpty()) {
        qDebug() << "报告生成失败，路径为空，请检查报告生成逻辑。";
        obj["message"] = StatusMessage::InternalServerError;
    } else {
        bool ret = DatabaseManager::instance().updateReportPath(patientName, packageName, packageDate, reportPath);
        if(ret) {
            obj["message"] = StatusMessage::GenerateReportSuccessfully;
            qDebug() << "成功";
        } else {
            obj["message"] = StatusMessage::InternalServerError;
        }
    }

    Packet packet =Protocol::createPacket(SaveReportResponce,obj);

    QByteArray array = Protocol::serializePacket(packet);

    socket->write(array);


}
