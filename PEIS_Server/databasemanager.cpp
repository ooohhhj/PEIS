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

bool DatabaseManager::updateUserPassword(const QString &phoneNumber, const QString &password)
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
        QString hashedPassword =hashPassword(password,salt);
        qDebug()<<"hashedPassword="<<hashedPassword;

        QSqlQuery query(db);
        QString sql ="update users set password =:password,salt =:salt where phone_number=:phone_number";

        query.prepare(sql);
        query.bindValue(":password",hashedPassword);
        query.bindValue(":salt",salt);
        query.bindValue(":phone_number",phoneNumber);

        if(!query.exec())
        {
            qDebug()<<"Error inserting user:"<<query.lastError();
            return false;
        }
        else
        {
            qDebug()<<"forgetPassword successfully";
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

bool DatabaseManager::findUserByPhoneNumber(const QString &phoneNumber)
{
    if(!isConnected())
    {
        qDebug()<<"Failed to connect to database:"<<db.lastError().text();
    }
    else
    {
        QSqlQuery query(db);
        query.prepare("SELECT * FROM users WHERE phone_number = :phone_number");
        query.bindValue(":phone_number",phoneNumber);

        if(!query.exec())
        {
            qDebug()<<"Failed to find user:"<<query.lastError().text();
            return false;// 查询失败时返回 false
        }
        else
        {
            if (query.next()) // 检查是否有结果返回
            {
                qDebug() << "User found!";
                return true; // 找到了用户，返回 true
            }
            else
            {
                qDebug() << "No user found with this phone number.";
                return false; // 没有找到用户，返回 false
            }
        }
    }
}

bool DatabaseManager::findUserByPhoneAndPassword(const QString &phoneNumber, const QString &password)
{
    //获取盐值
    QString salt = getSaltByPhoneNumber(phoneNumber);

    if(salt.isEmpty())
    {
        return false;
    }
    QString hashedPassword =hashPassword(password,salt);

    if(!isConnected()|| !db.isOpen() || !db.isValid())
    {
        qDebug()<<"Failed to connect to database:"<<db.lastError().text();
        return  false;
    }
    else
    {
        QSqlQuery query(db);
        query.prepare("select * from users where phone_number =:phone_number and password =:password");
        query.bindValue(":phone_number",phoneNumber);
        query.bindValue(":password",hashedPassword);

        if(query.exec())
        {
            if(query.next())
            {
                //手机号码与密码匹配
                return true;
            }
            else
            {
                //查无此人
                return false;
            }
        }
        else
        {
            qDebug() << "Query failed:" << query.lastError().text();
            return false;  // 查询失败时返回空字符串
        }
    }
}

bool DatabaseManager::findUserByUsernameAndPassword(const QString &username, const QString &password)
{

    //获取盐值
    QString salt = getSaltByUsername(username);

    if(salt.isEmpty())
    {
        return false;
    }
    QString hashedPassword =hashPassword(password,salt);

    if(!isConnected()|| !db.isOpen() || !db.isValid())
    {
        qDebug()<<"Failed to connect to database:"<<db.lastError().text();
        return  false;
    }
    else
    {
        QSqlQuery query(db);

        query.prepare("select * from users where username =:username and password =:password");
        query.bindValue(":username",username);
        query.bindValue(":password",hashedPassword);

        if(query.exec())
        {
            if(query.next())
            {
                //用户名与密码匹配
                return true;
            }
            else
            {
                //查无此人
                return false;
            }
        }
        else
        {
            qDebug()<<"ijd";
            qDebug() << "Query failed:" << query.lastError().text();
            return false;  // 查询失败时返回空字符串
        }
    }
}

QString DatabaseManager::getSaltByUsername(const QString &username)
{
    if(!isConnected())
    {
        qDebug()<<"Failed to connect to database:"<<db.lastError().text();
    }
    else
    {
        QSqlQuery query(db);
        query.prepare("select salt from users where username =:username");
        query.bindValue(":username",username);

        if(query.exec())
        {
            if(query.next())
            {
                return query.value(0).toString();//返回盐值
            }
            else
            {
                qDebug()<< "No matching user found.";
                return QString();  // 没有找到匹配用户时返回空字符串
            }
        }
        else
        {
            qDebug() << "Query failed:" << query.lastError().text();
            return QString();  // 查询失败时返回空字符串
        }
    }
}

QString DatabaseManager::getSaltByPhoneNumber(const QString &phoneNumber)
{
    if(!isConnected())
    {
        qDebug()<<"Failed to connect to database:"<<db.lastError().text();
    }
    else
    {
        QSqlQuery query(db);
        query.prepare("select salt from users where phone_number =:phone_number");
        query.bindValue(":phone_number",phoneNumber);

        if(query.exec())
        {
            if(query.next())
            {
                return query.value(0).toString();
            }
            else
            {
                qDebug()<< "No matching user found.";
                return QString();  // 没有找到匹配用户时返回空字符串
            }
        }
        else
        {

            qDebug() << "Query failed:" << query.lastError().text();
            return QString();  // 查询失败时返回空字符串
        }
    }
}

int DatabaseManager::getRoleIdByPhoneNumber(const QString &phoneNumber)
{
    if(!isConnected())
    {
        qDebug()<<"Failed to connect to database:"<<db.lastError().text();
    }
    else
    {
        QSqlQuery query(db);
        query.prepare("select role_id from users where phone_number = :phone_number");
        query.bindValue(":phone_number",phoneNumber);

        if(query.exec())
        {
            if(query.next())
            {
                return query.value(0).toInt();
            }
            else
            {
                qDebug()<< "No matching user found.";
                return -1;  // 没有找到匹配用户时返回空字符串
            }
        }
        else
        {

            qDebug() << "Query failed:" << query.lastError().text();
            return -1;  // 查询失败时返回空字符串
        }
    }
}

int DatabaseManager::getRoleIdByUsername(const QString &username)
{
    if(!isConnected())
    {
        qDebug()<<"Failed to connect to database:"<<db.lastError().text();
    }
    else
    {
        QSqlQuery query(db);
        query.prepare("select role_id from users where username =:username");
        query.bindValue(":username",username);

        if(query.exec())
        {
            if(query.next())
            {
                return query.value(0).toInt();
            }
            else
            {
                qDebug()<< "No matching user found.";
                return -1;  // 没有找到匹配用户时返回空字符串
            }
        }
        else
        {
            qDebug() << "Query failed:" << query.lastError().text();
            return -1;  // 查询失败时返回空字符串
        }
    }
}

QString DatabaseManager::getUsernameByPhoneNumber(const QString &phoneNumber)
{
    if(!isConnected())
    {
        qDebug()<<"Failed to connect to database:"<<db.lastError().text();
        return QString();
    }
    else
    {
        QSqlQuery query(db);

        query.prepare("select username from users where phone_number =:phone_number");
        query.bindValue(":phone_number",phoneNumber);

        if(query.exec())
        {
            if(query.next())
            {
                return query.value(0).toString();
            }
            else
            {
                qDebug()<< "No matching user found.";
                return QString();  //
            }
        }
        else
        {
            qDebug()<<"查询失败";
            qDebug() << "Query failed:" << query.lastError().text();
            return QString();   // 查询失败时返回空字符串
        }
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
