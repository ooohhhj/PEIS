#include "captchagenerator.h"


QString captchaGenerator::captchaText ="";

QPixmap captchaGenerator::refreshCaptcha()
{
    //生成随机字符串
    captchaText = generateRandomString(6);

    //创建图像
    //图像大小
    QPixmap captchaPixmap(120,40);
    captchaPixmap.fill(Qt::white);//填充白色背景

    //创建QPainter
    QPainter painter(&captchaPixmap);
    painter.setFont(QFont("Arial",10));

    //随机字符颜色和旋转
    for(int i=0;i<captchaText.size();i++)
    {
        //设置随机颜色
        QColor randomColor(QRandomGenerator::global()->bounded(256),
                           QRandomGenerator::global()->bounded(256),
                           QRandomGenerator::global()->bounded(256));

        painter.setPen(randomColor);

        //保存画布状态
        painter.save();

        //随机旋转字符
        int rotationAngle = QRandomGenerator::global()->bounded(-30,30);
        painter.translate(25+i*13,30);
        painter.rotate(rotationAngle);

        //绘制字符
        painter.drawText(0,0,QString(captchaText[i]));

        //恢复画布状态
        painter.restore();
    }

    //添加干扰线条
    for(int i=0;i<10;i++)
    {
        //设置随机颜色
        QColor lineColor(QRandomGenerator::global()->bounded(256),
                         QRandomGenerator::global()->bounded(256),
                         QRandomGenerator::global()->bounded(256));
        painter.setPen(lineColor);

        //随机位置绘制线条
        int x1=QRandomGenerator::global()->bounded(captchaPixmap.width());
        int y1=QRandomGenerator::global()->bounded(captchaPixmap.height());
        int x2=QRandomGenerator::global()->bounded(captchaPixmap.width());
        int y2=QRandomGenerator::global()->bounded(captchaPixmap.height());
        painter.drawLine(x1,y1,x2,y2);

    }


    //添加一些随机噪点
    for(int i=0;i<100;i++)
    {
        //设置随机颜色
        QColor pointColor(QRandomGenerator::global()->bounded(256),
                          QRandomGenerator::global()->bounded(256),
                          QRandomGenerator::global()->bounded(256));

        painter.setPen(pointColor);

        //随机位置绘制点
        int x=QRandomGenerator::global()->bounded(captchaPixmap.width());
        int y=QRandomGenerator::global()->bounded(captchaPixmap.height());
        painter.drawPoint(x,y);
    }
   return captchaPixmap;
}

QString captchaGenerator::getCaptchaText()
{
    return captchaText;
}

QString captchaGenerator::generateRandomString(int length)
{
    const QString possibleCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    QString randomString;
    for(int i=0;i<length;i++)
    {
        //生成随机字符
        int index= QRandomGenerator::global()->bounded(possibleCharacters.length());
        QChar nextChar =possibleCharacters.at(index);
        randomString.append(nextChar);//拼接字符串
    }
    return randomString;

}
