#include "databasemanager.h"

DatabaseManager::DatabaseManager()
{
    db = QSqlDatabase::addDatabase("QODBC");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("peis");
    db.setUserName("root");
    db.setPassword("root");
    db.setPort(3306);

    if(!db.open())
    {
        qDebug()<<"Failed to connect to database:"<<db.lastError().text();
    }
    else
    {
        qDebug()<<"Connected to batabase successfully";
    }
}

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::~DatabaseManager()
{
    if(db.isOpen())
    {
        db.close();
    }
}

bool DatabaseManager::insertUser(const QString &username, const QString &gender, const QString &birthdate, const QString &idCard, const QString &address, const QString &phoneNumber, const QString &password, const int &role_id)
{
    if(!isConnected())
    {
        qDebug()<<"Failed to connect to database:"<<db.lastError().text();
    }
    else
    {
        //生成盐值
        QString salt =generateSalt(16);

        //对密码进行加盐哈希加密
        QString hashedPassword = hashPassword(password,salt);
        qDebug()<<"hashedPassword ="<<hashedPassword ;

        QSqlQuery query(db);
        QString sql ="insert into users (username,gender,birth_date,id_card,address,phone_number,password,role_id,salt) "
                     "values(:username,:gender,:birth_date,:id_card,:address,:phone_number,:password,:role_id,:salt)";

        query.prepare(sql);
        query.bindValue(":username",username);
        query.bindValue(":gender",gender);
        query.bindValue(":birth_date",birthdate);
        query.bindValue(":id_card",idCard);
        query.bindValue(":address",address);
        query.bindValue(":phone_number",phoneNumber);
        query.bindValue(":password",hashedPassword);
        query.bindValue(":role_id",role_id);
        query.bindValue(":salt",salt);

        if(!query.exec())
        {
            qDebug()<<"Error inserting user:"<<query.lastError();
            return false;
        }
        else
        {
            qDebug()<<"User registered successfully";
            return true;
        }
    }
}

QSqlQuery DatabaseManager::findUserByName(const QString &username)
{
    if(!isConnected())
    {
        qDebug()<<"Failed to connect to database:"<<db.lastError().text();
    }
    else
    {

        QSqlQuery query(db);
        query.prepare("select * from users where username = :username ");
        query.bindValue(":username",username);

        if(!query.exec())
        {
            qDebug()<<"Failed to find user:"<<query.lastError().text();
        }
        return query;
    }
}

bool DatabaseManager::isConnected() const
{
    return db.isOpen();
}

QString DatabaseManager::generateSalt(int length)
{
    //生成盐值
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    QString salt;
    for(int i=0;i<length;i++)
    {
        int index =QRandomGenerator::global()->bounded(possibleCharacters.length());
        salt.append(possibleCharacters.at(index));
    }
    return salt;
}

QString DatabaseManager::hashPassword(const QString &password, const QString &salt)
{
    QByteArray saltedPassword =(password + salt).toUtf8();
    QByteArray hash = QCryptographicHash::hash(saltedPassword,QCryptographicHash::Sha256);
    return QString(hash.toHex());
}
