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
    // 获取患者基本信息
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
    QString reportDirectory = QString("H:/PEIS/PEIS/reports/%1/%2/")
            .arg(patient_Name)
            .arg(package_name);

    // 创建文件夹，如果不存在的话
    QDir dir(reportDirectory);
    if (!dir.exists()) {
        dir.mkpath(reportDirectory);
    }

    // 生成报告文件路径
    QString reportPath = QString("%1%2_report.pdf")
            .arg(reportDirectory)
            .arg(QDate::currentDate().toString("yyyyMMdd"));



    // 创建打印机对象
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(reportPath);
    printer.setPageSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Portrait);

    // 创建绘图工具
    QPainter painter(&printer);
    auto setFont = [&](const QString& fontName, int size, QFont::Weight weight = QFont::Normal) {
        QFont font(fontName, size, weight);
        painter.setFont(font);
    };

    // 绘制前面的空行
    // 绘制前面的空行
    int emptyLinesBefore = 2; // 前面空行数量
    int lineSpacing = painter.fontMetrics().lineSpacing();
    painter.translate(0, emptyLinesBefore * lineSpacing); // 垂直移动绘图位置

    // 绘制标题
    setFont("Arial", 20, QFont::Bold);
    QString title = "体检报告";
    QFontMetrics fontMetrics(painter.font());
    int titleWidth = fontMetrics.horizontalAdvance(title);
    int pageWidth = printer.pageRect().width();
    int titleX = (pageWidth - titleWidth) / 2;

    painter.drawText(titleX, 50, title);

    // 更新Y坐标
    lineSpacing = fontMetrics.lineSpacing();
    int infoY = 50 + lineSpacing + 20; // 标题底部位置加上行距和额外的空白

    // 绘制分隔线
    // 绘制分隔线
    int lineY = infoY; // 保存当前的 Y 坐标
    if (lineY < printer.pageRect().height()) { // 确保分隔线在页面范围内
        painter.drawLine(50, lineY, pageWidth - 50, lineY);
    }
    infoY += 30; // 分隔线底部的间隔


    // 绘制基本信息
    setFont("Arial", 16);
    painter.drawText(100, infoY, "患者姓名: " + patient_Name + "                    性别: " + gender);
    infoY += lineSpacing + 5; // 下移一行

    painter.drawText(100, infoY, "出生日期: " + birth_date + "                  电话: " + phone_number);
    infoY += lineSpacing + 5; // 下移一行

    painter.drawText(100, infoY, "套餐名称: " + package_name);
    infoY += lineSpacing + 5; // 下移一行

    painter.drawText(100, infoY, "套餐日期: " + package_date + "                报告生成时间: " + report_generated);
    infoY += lineSpacing + 50; // 下移一些空白

    // 绘制“体检项目结果”标题
    setFont("Arial", 18, QFont::Bold);
    QString resultTitle = "体检项目结果";
    int resultTitleX = (pageWidth - fontMetrics.horizontalAdvance(resultTitle)) / 2;
    painter.drawText(resultTitleX, infoY, resultTitle);
    infoY += lineSpacing + 10; // 下移一行

    // 绘制分隔线
    painter.drawLine(100, infoY, pageWidth - 100, infoY);
    infoY += 30; // 下移到数据行

    // 设置内容字体
    setFont("Arial", 16);

    // 绘制表头
    QStringList headers = { "项目名称", "输入数据", "正常范围", "结果日期", "责任人" };
    QVector<int> columnWidths(headers.size(), 0);

    // 计算列宽
    for (const QString &header : headers) {
        columnWidths.push_back(fontMetrics.horizontalAdvance(header));
    }

    setFont("Arial", 10);
    // 计算起始X坐标
    int startX = 100; // 表格左边距
    int columnSpacing = 2500; // 列间距

    // 绘制表头
    for (int i = 0; i < headers.size(); ++i) {
        painter.drawText(startX + i * (columnWidths[i] + columnSpacing), infoY, headers[i]);
    }
    infoY += lineSpacing + 5; // 下移表头下方的间隔

    // 绘制表头下方的横线
    painter.drawLine(100, infoY, pageWidth - 100, infoY);
    infoY += 10; // 下移到数据行

    // 处理每一个体检项目
    for (const QJsonValue &value : dateArray) {
        QJsonObject dateObject = value.toObject();
        QString itemName = dateObject["item_name"].toString();
        QString inputData = dateObject["input_data"].toString();
        QString normalRange = dateObject["normal_range"].toString();
        QString resultDate = dateObject["result_date"].toString();
        QString responsiblePerson = dateObject["responsible_person"].toString();

        // 绘制每一行的数据
        QStringList rowData = { itemName, inputData, normalRange, resultDate, responsiblePerson };
        for (int i = 0; i < rowData.size(); ++i) {
            painter.drawText(startX + i * (columnWidths[i] + columnSpacing), infoY, rowData[i]);
        }

        infoY += lineSpacing + 5; // 下移到下一行

        // 检查是否需要分页
        if (infoY >= printer.pageRect().height() - 100) {
            printer.newPage();
            infoY = 50; // 重置Y坐标
            setFont("Arial", 16, QFont::Bold); // 重新绘制标题
            painter.drawText(titleX, 50, title);
            painter.drawLine(50, 70, pageWidth - 50, 70);
            infoY += 10; // 更新Y坐标

            // 重新绘制基本信息
            setFont("Arial", 12);
            painter.drawText(100, infoY, "患者姓名: " + patient_Name + "   性别: " + gender);
            infoY += lineSpacing + 5; // 下移一行

            painter.drawText(100, infoY, "出生日期: " + birth_date + "   电话: " + phone_number);
            infoY += lineSpacing + 5; // 下移一行

            painter.drawText(100, infoY, "套餐名称: " + package_name);
            infoY += lineSpacing + 5; // 下移一行

            painter.drawText(100, infoY, "套餐日期: " + package_date + "   报告生成时间: " + report_generated);
            infoY += lineSpacing + 20; // 下移一些空白

            // 重新绘制“体检项目结果”标题
            setFont("Arial", 14, QFont::Bold);
            painter.drawText(resultTitleX, infoY, resultTitle);
            infoY += lineSpacing + 10; // 下移一行

            // 绘制分隔线
            painter.drawLine(100, infoY, pageWidth - 100, infoY);
            infoY += 10; // 下移到数据行

            // 绘制表头
            for (int i = 0; i < headers.size(); ++i) {
                painter.drawText(startX + i * (columnWidths[i] + columnSpacing), infoY, headers[i]);
            }
            infoY += lineSpacing + 5; // 下移表头下方的间隔

            // 绘制表头下方的横线
            painter.drawLine(100, infoY, pageWidth - 100, infoY);
            infoY += 10; // 下移到数据行
        }
    }

    // 为医生姓名和建议添加间隔
    infoY += 100;

    // 绘制医生姓名和建议
    setFont("Arial", 16);
    painter.drawText(100, infoY, "医生姓名: " + doctor_name);
    infoY += lineSpacing + 10; // 下移一行

    // 绘制“医生建议”标题
    painter.drawText(100, infoY, "医生建议：");
    infoY += lineSpacing + 10; // 下移一行

    // 使用 QTextDocument 来绘制长文本
    QTextDocument textDoc;
    textDoc.setDefaultFont(painter.font());
    textDoc.setPlainText(doctor_advice);

    // 计算文本区域的宽度
    int textWidth = pageWidth - 200;

    // 设置 QTextDocument 的宽度
    textDoc.setTextWidth(textWidth);

    // 保存当前 painter 的状态
    painter.save();

    // 将 painter 移动到正确的位置
    painter.translate(100, infoY);

    // 绘制 QTextDocument
    textDoc.drawContents(&painter);

    // 恢复 painter 的状态
    painter.restore();

    // 更新 infoY 位置
    infoY += textDoc.size().height() + 10; // 根据文本高度更新 Y 坐标

    painter.end(); // 结束绘制
    return reportPath; // 返回报告文件路径
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

            connect(handler,&ClientHandler::InsertUserMapById,this,&ServerSocket::InsertUserMapById);

            connect(handler,&ClientHandler::ForwardDoctorMessage,this,&ServerSocket::ForwardDoctorMessage);



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

void ServerSocket::InsertUserMapById(const int &userId, QTcpSocket *clientSocket)
{
    // 加锁
    QWriteLocker  locker(&rwLock);
    if (!UserConnections.contains(userId)) {
        UserConnections.insert(userId, clientSocket);
        qDebug() << "Inserted userId=" << userId << " into UserConnections";
    } else {
        qDebug() << "userId=" << userId << " already exists in doctorConnections";
    }
}

void ServerSocket::ForwardDoctorMessage(const int &doctorId)
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
    QString reportPath = generateHealthCheckupReport(query, dateArray);

    QString message;
    // 检查报告生成是否成功
    if(reportPath.isEmpty())
    {
        message = StatusMessage::InternalServerError;
    }
    else
    {
        bool ret = DatabaseManager::instance().updateReportPath(patientName, packageName, packageDate, reportPath);

        bool ret2 =DatabaseManager::instance().updateAppointMents(patientName, packageName, packageDate);

        if(ret && ret2 ) {
            message = StatusMessage::GenerateReportSuccessfully;

            //转发消息给用户
            int userId = DatabaseManager::instance().getUserIdByUsername(patientName);

            UserForwardMessage(userId);

        }
        else {
            message = StatusMessage::InternalServerError;
        }
    }

    QJsonObject obj;

    obj["message"] = message;

    Packet packet =Protocol::createPacket(SaveReportResponce,obj);

    QByteArray array = Protocol::serializePacket(packet);

    socket->write(array);

}

void ServerSocket::UserForwardMessage(const int &userId)
{
    QReadLocker locker(&rwLock);
    if (UserConnections.contains(userId))
    {
        QTcpSocket *doctorSocket = UserConnections[userId];
        //发送
        QJsonObject obj;

        Packet packet =Protocol::createPacket(UserCheckupGenerateNotice,obj);

        QByteArray array =Protocol::serializePacket(packet);

        doctorSocket->write(array);
    }

}
