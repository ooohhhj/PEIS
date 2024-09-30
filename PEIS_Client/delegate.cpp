#include "delegate.h"


QWidget *Delegate::createEditor(QWidget *parent, const QStyleOptionViewItem & /*option*/, const QModelIndex &index) const
{
    QPushButton *button = new QPushButton(parent);

    QString status = index.model()->index(index.row(), statusColumn).data().toString();  // 使用 statusColumn 代替硬编码的 4


    if (status == "已体检") {
        button->setText("编辑报告");
    } else if (status == "已完成") {
        button->setText("查看报告");
    } else {
        button->setText("未知操作");
    }

    // 连接按钮点击信号到槽函数
    connect(button, &QPushButton::clicked, this, [this, index]() {
        emit buttonClicked(index);  // 发射信号
    });

    // 设置样式表
    button->setStyleSheet(
                "QPushButton {"
                "   background-color: #4CAF50;"  // 背景色
                "   color: white;"               // 字体颜色
                "   border: none;"               // 无边框
                "   padding: 5px 10px;"          // 内边距
                "   text-align: center;"         // 文字居中
                "   text-decoration: none;"      // 无下划线
                "   display: inline-block;"
                "   font-size: 14px;"            // 字体大小
                "   margin: 4px 2px;"            // 外边距
                "   border-radius: 10px;"        // 圆角边框
                "   transition-duration: 0.4s;"  // 过渡时间
                "}"
                "QPushButton:hover {"
                "   background-color: #45a049;"  // 鼠标悬停时的背景色
                "}"
                );

    return button;
}

void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString status = index.model()->index(index.row(), statusColumn).data().toString();  // 使用 statusColumn 代替硬编码的 4


    QPushButton button;
    if (status == "已体检") {
        button.setText("编辑报告");
    } else if (status == "已完成") {
        button.setText("查看报告");
    } else {
        button.setText("未知操作");
    }

    button.resize(option.rect.size());

    // 将按钮的外观绘制到表格项中
    QPixmap pixmap = button.grab();
    painter->drawPixmap(option.rect.topLeft(), pixmap);
}
