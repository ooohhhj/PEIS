#ifndef CAPTCHAGENERATOR_H
#define CAPTCHAGENERATOR_H

#include<QLabel>
#include <QRandomGenerator>
#include<QPainter>

class captchaGenerator : public QLabel
{
    Q_OBJECT
public:
    captchaGenerator(QWidget *parent = nullptr):QLabel(parent)
    {
        //初始化生成验证码并显示
        refreshCaptcha();
    }

   static QPixmap refreshCaptcha();

    static QString getCaptchaText();
signals:

private:
   static QString generateRandomString(int length);
private:
   static QString captchaText;// 静态成员变量保存验证码文本
};

#endif // CAPTCHAGENERATOR_H
