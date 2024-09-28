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

    void requestPackageInfo();

    void setUsername(const QString& username );

private slots:
    void on_exitButton_clicked();
    void on_reserveButton_clicked();
    void OnUpdateUserAppointment(const QString& selectdate);

signals:
    void exitButtonClicked();

private:
    Ui::HEPDetails *ui;
    QString m_cardName;
    QString m_username;
};

#endif // HEPDETAILS_H
