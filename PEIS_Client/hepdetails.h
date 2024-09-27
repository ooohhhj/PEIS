#ifndef HEPDETAILS_H
#define HEPDETAILS_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QDate>
#include <QHBoxLayout>
#include <QDebug>
#include <QFile>
#include <QTextEdit>

#include "clientsocket.h"

namespace Ui {
class HEPDetails;
}

class HEPDetails : public QWidget
{
    Q_OBJECT

public:
    explicit HEPDetails(QWidget *parent = nullptr);
    ~HEPDetails();

    void setupSchedule();

    void setCardName(const QString& cardName);

    void setPackageInfo(const QJsonArray & packageInfo,const QString &packageDescription);

    void setExamination();

    void setInstructions(const QString & filePath);

private slots:
    void on_exitButton_clicked();

signals:
    void exitButtonClicked();

private:
    Ui::HEPDetails *ui;
    QString m_cardName;
};

#endif // HEPDETAILS_H
