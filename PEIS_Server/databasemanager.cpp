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
            return QSqlQuery();
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

QSqlQuery DatabaseManager::getReserveCheckup(const int& itemsPerPage,const int& offset)
{
    if(!isConnected())
    {
        qDebug()<<"Failed to connect to database:"<<db.lastError().text();
        return QSqlQuery();
    }

    QString sql = QString("SELECT package_name, target_population, provider, price "
                          "FROM healthpackages "
                          "LIMIT %1 OFFSET %2").arg(itemsPerPage).arg(offset);


    QSqlQuery query(db);
    if(!query.exec(sql))
    {
        qDebug() << "Query execution failed:" << query.lastError().text();
        return QSqlQuery();
    }

    // 返回查询结果
    return query;
}

QSqlQuery DatabaseManager::getPackageNameInfo(const QString &packageName)
{
    if(!isConnected())
    {
        qDebug()<<"Failed to connect to database:"<<db.lastError().text();
        return QSqlQuery();
    }

    QSqlQuery query(db);
    query.prepare("SELECT hi.item_name, hi.item_description, hp.description AS package_description "
                  "FROM healthpackages hp "
                  "JOIN packageitemmapping pim ON hp.id = pim.package_id "
                  "JOIN healthitems hi ON pim.item_id = hi.id "
                  "WHERE hp.package_name = :packageName");
    query.bindValue(":packageName",packageName);
    if(!query.exec())
    {
        qDebug() << "Query failed: " << query.lastError();
        return QSqlQuery();
    }
    return query;
}

QSqlQuery DatabaseManager::searchPackageNameInfo(const QString &searchPackageName, int offset, int itemsPerPage)
{
    if(!isConnected())
    {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return QSqlQuery();
    }

    QSqlQuery query(db);
    query.prepare("SELECT hp.package_name, hp.target_population, hp.provider, hp.price "
                  "FROM healthpackages hp "
                  "WHERE hp.package_name LIKE :packageName "
                  "LIMIT :offset, :itemsPerPage");

    //使用模糊匹配
    query.bindValue(":packageName", "%" + searchPackageName + "%");
    query.bindValue(":offset", offset);
    query.bindValue(":itemsPerPage", itemsPerPage);


    if(!query.exec())
    {
        qDebug() << "Query failed: " << query.lastError();
        return QSqlQuery();
    }
    return query;
}

int DatabaseManager::calculateTotalPages(int itemsPerPage)
{
    if(!isConnected())
    {
        qDebug()<<"Failed to connect to database:"<<db.lastError().text();
        return -1;
    }

    int totalItems;

    QString sql ="select count(*) from healthpackages";
    QSqlQuery query(db);
    if(!query.exec(sql))
    {
        qDebug() << "Query execution failed:" << query.lastError().text();
        return -1;
    }
    else
    {
        if(query.next())
        {
            totalItems = query.value(0).toInt();
        }

    }


    qDebug()<<"totalItems="<<totalItems;
    //计算总页数
    return (totalItems+itemsPerPage -1)/itemsPerPage;// 向上取整
}

int DatabaseManager::getPackageCount(const QString &packageName)
{
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return 0;
    }

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM healthpackages WHERE package_name LIKE :packageName");
    query.bindValue(":packageName", "%" + packageName + "%");

    if (!query.exec() || !query.next()) {
        qDebug() << "Query failed: " << query.lastError();
        return 0;
    }

    return query.value(0).toInt();
}

int DatabaseManager::getAppointmentCount(const int &packageId, const QString &selectedDate)
{
    //查询套餐相同日期的数量
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return 0;
    }

    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) AS appointment_count "
                  "FROM appointments "
                  "WHERE packageId = :packageId "
                  "AND DATE(appointment_date) = DATE(:date)");

    query.bindValue(":packageId",packageId);
    query.bindValue(":date",selectedDate);

    if(query.exec() && query.next())
    {
        return query.value(0).toInt();
    }

    return 0;
}


bool DatabaseManager::isUserAlreadyByAppointments(const int &userId, const int &packageId, const QString &appointmentDate)
{
    //判断用户是否预约了
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM appointments "
                  "WHERE user_id = :userId "
                  "AND package_id = :packageId "
                  "AND DATE(appointment_date) = DATE(:selectedDate)");

    query.bindValue(":username",userId);
    query.bindValue(":cardName",packageId);
    query.bindValue(":selectedDate",appointmentDate);

    if(query.exec()&&query.next())
    {
        return query.value(0).toInt()>0;
    }

    return false;
}

int DatabaseManager::getAvailablePackageCount(const QString &cardName, const QString &selecteDate)
{
    //获取套餐的上限数量
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return 0;
    }

    QSqlQuery query(db);
    query.prepare("select available_slots from healthpackages where package_name = :cardName");
    query.bindValue(":cardName",cardName);

    if(query.exec()&&query.next())
    {
        return query.value(0).toInt();
    }
    return -1;
}

bool DatabaseManager::insertAppointment(const int &userId, const int &packageId, const QString &appointmentDate)
{
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return 0;
    }

    // 分配医生
    int doctorId = getDoctorByDepartment(packageId);
    if (doctorId == -1) {
        qDebug() << "No doctor available for this package.";
        return false;  // 无法分配医生，插入失败
    }

    QSqlQuery query(db);

    // 插入预约并分配医生
    query.prepare("INSERT INTO appointments (user_id, package_id, appointment_date, status, doctor_id) "
                  "VALUES (:userId, :packageId, :appointmentDate, :status, :doctorId)");

    query.bindValue(":userId", userId);
    query.bindValue(":packageId", packageId);
    query.bindValue(":appointmentDate", appointmentDate);
    query.bindValue(":status", "已预约");
    query.bindValue(":doctorId", doctorId);

    // 执行插入
    if (query.exec()) {
        qDebug() << "Appointment inserted successfully.";
        return true; // 插入成功，返回 true
    } else {
        qDebug() << "Failed to insert appointment:" << query.lastError().text();
        return false; // 插入失败，返回 false
    }
}

int DatabaseManager::getUserIdByUsername(const QString &username)
{
    if(!isConnected())
    {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return -1; // 返回 -1 表示未找到用户或数据库连接失败
    }

    QSqlQuery query(db);
    query.prepare("SELECT id FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    } else {
        qDebug() << "Query failed or user not found: " << query.lastError().text();
        return -1;
    }
}

int DatabaseManager::getPackageIdByName(const QString &packageName)
{
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return -1; // 返回 -1 表示未找到套餐或数据库连接失败
    }

    QSqlQuery query(db);
    query.prepare("SELECT id FROM healthpackages WHERE package_name = :packageName");
    query.bindValue(":packageName", packageName);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    } else {
        qDebug() << "Query failed or package not found: " << query.lastError().text();
        return -1;
    }
}

int DatabaseManager::getDoctorByDepartment(const int &packageId)
{
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return -1; // 返回 -1 表示出错
    }

    QSqlQuery query(db);

    // 通过 packageId 查找对应的 department_id，并随机选择一个医生
    query.prepare("SELECT d.id "
                  "FROM doctors d "
                  "JOIN departments dept ON d.department_id = dept.id "
                  "JOIN healthpackages hp ON hp.department_id = dept.id "
                  "WHERE hp.id = :packageId "
                  "ORDER BY RAND() LIMIT 1");  // 随机分配一个医生

    query.bindValue(":packageId", packageId);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();  // 返回医生 ID
    } else {
        qDebug() << "Failed to find doctor for the package:" << query.lastError().text();
        return -1; // 返回 -1 表示分配失败
    }
}

QSqlQuery DatabaseManager::getAppointmentsByusername(const QString &username)
{
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return QSqlQuery(); // 返回 -1 表示出错
    }

    QSqlQuery query(db);

    int userId = getUserIdByUsername(username);
    if (userId == -1) {
        qDebug() << "Doctor with username" << username << "not found.";
        return QSqlQuery(); // 返回空查询表示出错
    }

    qDebug()<<"调试";
    //获取到预约医生为userid的病人信息// 查询预约信息，包括病人信息、体检套餐、体检日期和状态
    query.prepare("SELECT p.username AS patient_name, "
                  "p.phone_number AS patient_phone, "
                  "h.package_name AS health_package, "
                  "a.appointment_date, "
                  "a.status AS appointment_status "
                  "FROM appointments a "
                  "JOIN users p ON a.user_id = p.id "  // 关联预约表中的病人ID
                  "JOIN healthpackages h ON a.package_id = h.id "  // 关联体检套餐
                  "JOIN doctors d ON a.doctor_id = d.id "  // 关联医生ID
                  "WHERE d.user_id = :userId;");  //  通过医生的 user_id 来查

    query.bindValue(":userId", userId);
    if (!query.exec()) {
        qDebug() << "Error retrieving appointment information:" << query.lastError();
        return QSqlQuery(); // 返回空查询表示出错
    }

    return query; // 成功执行查询，返回结果
}

QString DatabaseManager::getStartDateByusername(const QString &username)
{
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return QString();
    }

    QSqlQuery query(db);

    int userId =getUserIdByUsername(username);

    query.prepare("SELECT MIN(a.appointment_date) AS first_appointment_date "
                  "FROM appointments a "
                  "JOIN doctors d ON a.doctor_id = d.id "
                  "WHERE d.user_id = :userId");
    query.bindValue(":userId", userId);

    if (query.exec()) {
        if (query.next()) {
            QString firstAppointmentDate = query.value(0).toString();
            return firstAppointmentDate;
        }
    } else {
        qDebug() << "Query execution failed:" << query.lastError();
        return QString();
    }

}

QSqlQuery DatabaseManager::getUserInfoByUsername(const QString &username)
{
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return QSqlQuery();
    }

    int userId =getUserIdByUsername(username);
    if (userId == -1) {
        qDebug() << "Patient with username" << username << "not found.";
        return QSqlQuery(); // 返回空查询表示出错
    }

    QSqlQuery query(db);
    query.prepare("SELECT u.username AS patient_name, "
                  "u.gender AS patient_gender, "
                  "u.phone_number AS patient_phone, "
                  "u.birth_date AS patient_birth_date, "
                  "h.package_name AS health_package, "
                  "a.appointment_date, "
                  "a.status AS appointment_status "
                  "FROM appointments a "
                  "JOIN users u ON a.user_id = u.id "
                  "JOIN healthpackages h ON a.package_id = h.id "
                  "WHERE u.id = :userId");

    query.bindValue(":userId", userId);
    if (!query.exec()) {
        qDebug() << "Failed to retrieve appointment data:" << query.lastError().text();
        return QSqlQuery();  // 查询执行失败，返回空查询
    }
    return query;
}


bool DatabaseManager::isConnected() const
{
    return db.isOpen();
}

QSqlQuery DatabaseManager::getUserInfosByRoleId(const int &role_id)
{
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return QSqlQuery();
    }

    QSqlQuery query(db);

    // 编写SQL查询语句，选择需要显示的用户信息
    query.prepare("SELECT users.id, users.username, users.gender, users.birth_date, "
                  "users.id_card, users.address, users.phone_number, roles.name AS role_name, "
                  "users.created_at, users.updated_at "
                  "FROM users "
                  "JOIN roles ON users.role_id = roles.id "
                  "WHERE users.role_id = :role_id");

    // 绑定参数
    query.bindValue(":role_id", role_id);

    // 执行查询
    if (!query.exec()) {
        qDebug() << "Query failed:" << query.lastError().text();
        return QSqlQuery();
    }

    return query;

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
