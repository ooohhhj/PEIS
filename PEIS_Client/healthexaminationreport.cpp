#include "healthexaminationreport.h"
#include "ui_healthexaminationreport.h"

HealthExaminationReport::HealthExaminationReport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HealthExaminationReport)
{
    ui->setupUi(this);

    connect(ClientSocket::instance(),&ClientSocket::OnGetHealthExaminationRePortResponce,this,&HealthExaminationReport::OnGetHealthExaminationRePortResponce);
}

HealthExaminationReport::~HealthExaminationReport()
{
    delete ui;
}

void HealthExaminationReport::displayPdf(const QString &fileName)
{

    // 创建新的 QScrollArea
    QScrollArea *scrollArea = new QScrollArea(ui->widget);
    scrollArea->setWidgetResizable(true); // 设置为可调整大小
    scrollArea->setStyleSheet("background-color: transparent;"
                              "border: none;");
    // 检查并清除旧布局
    QLayout *oldLayout = ui->widget->layout();
    if (oldLayout) {
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget(); // 删除旧的 widget
            delete item; // 删除布局项
        }
        delete oldLayout; // 删除旧布局
    }

    // 设置新的布局
    QVBoxLayout *newLayout = new QVBoxLayout(ui->widget);
    newLayout->addWidget(scrollArea); // 将 scrollArea 添加到 widget

    // 加载 PDF 文档
    Poppler::Document *document = Poppler::Document::load(fileName);
    if (!document || document->isLocked()) {
        QLabel *label = new QLabel("无法加载PDF文件", ui->widget);
        scrollArea->setWidget(label);
        return;
    }

    // 显示 PDF 每一页
    QVBoxLayout *layout = new QVBoxLayout();
    for (int i = 0; i < document->numPages(); ++i) {
        Poppler::Page *page = document->page(i);
        if (page) {
            QLabel *pageLabel = new QLabel(ui->widget);

            // 设置高分辨率 DPI
            const int dpi = 300; // 你可以调整这个值
            QImage pageImage = page->renderToImage(dpi, dpi);

            if (!pageImage.isNull()) {
                // 将图像缩放并居中显示
                pageLabel->setPixmap(QPixmap::fromImage(pageImage).scaled(scrollArea->size() * 50, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // 放大
                pageLabel->setAlignment(Qt::AlignCenter); // 居中对齐
                layout->addWidget(pageLabel); // 每页的 QLabel 添加到布局中
            } else {
                qDebug() << "无法渲染页面:" << i;
            }

            delete page; // 清理页面对象
        }
    }

    // 设置滚动区域的内容
    QWidget *container = new QWidget(ui->widget);
    container->setStyleSheet("background-color: transparent;\nborder: none;");

    container->setLayout(layout);
    scrollArea->setWidget(container);
//    scrollArea->setMinimumSize(800, 500); // 设置最小大小
}

void HealthExaminationReport::setSignalFlag(const int &flag)
{
    this->m_flag = flag;
}

void HealthExaminationReport::on_returnBtn_clicked()
{
    emit exitButtonClicked(m_flag);
}

void HealthExaminationReport::OnGetHealthExaminationRePortResponce(const QJsonObject &reprotPdfDate)
{
    QString fileName = reprotPdfDate["fileName"].toString();
    QString fileType = reprotPdfDate["fileType"].toString();
    QString base64Data = reprotPdfDate["fileData"].toString();

    QByteArray pdfData = QByteArray::fromBase64(base64Data.toUtf8());  // 解码 Base64

    // 根据文件类型处理
    if (fileType == "application/pdf") {
        QFile outFile(fileName);
        if (outFile.open(QIODevice::WriteOnly)) {
            outFile.write(pdfData);
            outFile.close();
        } else {
            qDebug() << "Failed to save PDF file:" << outFile.errorString();
            return;  // 如果保存失败，退出函数
        }
    } else {
        qDebug() << "Unsupported file type:" << fileType;
        return;  // 如果文件类型不支持，退出函数
    }

    // 将 PDF 显示在 widget 中
    displayPdf(fileName);

}

