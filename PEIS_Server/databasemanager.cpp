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
    // 判断用户是否预约了
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT status FROM appointments "
                  "WHERE user_id = :userId "
                  "AND package_id = :packageId "
                  "AND DATE(appointment_date) = DATE(:selectedDate)");

    // 绑定参数
    query.bindValue(":userId", userId);
    query.bindValue(":packageId", packageId);
    query.bindValue(":selectedDate", appointmentDate);

    // 执行查询
    if (!query.exec()) {
        qDebug() << "Failed to execute select query:" << query.lastError().text();
        return false;  // 查询失败
    }

    // 如果找到记录
    if (query.next()) {
        QString status = query.value(0).toString();  // 获取状态

        // 只要不是已取消的状态，都返回 true
        if (status == "已取消") {
            return false;
        } else {
            return true;
        }
    }

    // 没有匹配的记录，返回 false
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
                  "VALUES (:userId, :packageId, :appointmentDate, :status, :doctorId) "
                  "ON DUPLICATE KEY UPDATE "
                  "appointment_date = :appointmentDate, status = :status, doctor_id = :doctorId");


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

int DatabaseManager::getdoctorIdByDoctorTable(const int &userId)
{
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return 0;
    }

    // 创建数据库查询
    QSqlQuery query(db);

    // 准备 SQL 查询语句
    query.prepare("SELECT id FROM doctors WHERE user_id = :userId");
    query.bindValue(":userId", userId);

    // 执行查询
    if (!query.exec()) {
        qDebug() << "查询医生 ID 时出错:" << query.lastError().text();
        return -1; // 返回 -1 表示查询失败
    }

    // 检查是否找到了记录
    if (query.next()) {
        return query.value(0).toInt(); // 返回医生 ID
    } else {
        qDebug() << "未找到对应的医生 ID，userId:" << userId;
        return -1; // 返回 -1 表示未找到医生
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

QString DatabaseManager::getUsernameByuserId(const int &userid)
{
    if (!isConnected()) {
        qDebug() << "数据库连接失败:" << db.lastError().text();
        return QString(); // 返回 -1 表示连接失败
    }


    QSqlQuery query(db);

    query.prepare("select username from users where id = :userId");
    query.bindValue(":userId",userid);

    if (query.exec()) {
        if (query.next()) { // 检查是否有结果
            return query.value("username").toString(); // 返回用户名
        }
    } else {
        // 处理错误
        qWarning() << "查询失败:" << query.lastError().text();
    }

    return QString(); // 如果没有找到用户或查询失败，返回空字符串

}

QString DatabaseManager::getPackageNameBypackageId(const int &packageId)
{
    if (!isConnected()) {
        qDebug() << "数据库连接失败:" << db.lastError().text();
        return QString(); // 返回 -1 表示连接失败
    }


    QSqlQuery query(db);

    query.prepare("select package_name from healthpackages where id = :packageId");
    query.bindValue(":packageId",packageId);
    if (query.exec()) {
        if (query.next()) { // 检查是否有结果
            return query.value("package_name").toString(); // 返回用户名
        }
    } else {
        // 处理错误
        qWarning() << "查询失败:" << query.lastError().text();
    }

    return QString(); // 如果没有找到用户或查询失败，返回空字符串

}

int DatabaseManager::getDoctorIdByAppointments(const int &patientId, const int &packageId, const QString &appointmentDate)
{
    if (!isConnected()) {
        qDebug() << "数据库连接失败:" << db.lastError().text();
        return -1; // 返回 -1 表示连接失败
    }

    // 准备查询医生 ID 的 SQL 语句
    QSqlQuery query(db);
    query.prepare("SELECT doctor_id FROM appointments "
                  "WHERE user_id = :patientId AND package_id = :packageId AND appointment_date = :appointmentDate");

    query.bindValue(":patientId", patientId);
    query.bindValue(":packageId", packageId);
    query.bindValue(":appointmentDate", appointmentDate);

    // 执行查询
    if (!query.exec()) {
        qDebug() << "查询医生 ID 时出错:" << query.lastError().text();
        return -1; // 返回 -1 表示查询出错
    }

    // 检查结果并返回医生 ID
    if (query.next()) {
        return query.value("doctor_id").toInt(); // 返回找到的医生 ID
    } else {
        qDebug() << "未找到匹配的预约记录";
        return -1; // 返回 -1 表示未找到记录
    }
}

bool DatabaseManager::InsertHealthCheckData(QJsonObject &healthcheckupDate)
{
    QString patientName = healthcheckupDate["patientName"].toString();
    QString gender = healthcheckupDate["gender"].toString();
    QString birthDate = healthcheckupDate["birthDate"].toString();
    QString phoneNumber = healthcheckupDate["phoneNumber"].toString();
    QString packageName = healthcheckupDate["packageName"].toString();
    QString packageDate = healthcheckupDate["packageDate"].toString();
    QString status = healthcheckupDate["status"].toString();

    QJsonArray HealthCheckupDate = healthcheckupDate["HealthCheckupDate"].toArray();

    if (!isConnected()) {
        qDebug() << "数据库连接失败:" << db.lastError().text();
        return false;
    }

    // 开始事务
    db.transaction();

    // 插入主要健康检查信息
    QSqlQuery query(db);
    query.prepare("INSERT INTO health_checkup (patient_name, gender, birth_date, phone_number, package_name, package_date, report_status) "
                  "VALUES (:patientName, :gender, :birthDate, :phoneNumber, :packageName, :packageDate, :reportStatus)");

    query.bindValue(":patientName", patientName);
    query.bindValue(":gender", gender);
    query.bindValue(":birthDate", birthDate);
    query.bindValue(":phoneNumber", phoneNumber);
    query.bindValue(":packageName", packageName);
    query.bindValue(":packageDate", packageDate);
    query.bindValue(":reportStatus", status);

    // 执行查询
    if (!query.exec()) {
        qDebug() << "插入 health_checkup 时出错:" << query.lastError().text();
        db.rollback(); // 回滚事务
        return false;
    }

    // 获取插入后生成的健康检查 ID
    int healthCheckupId = query.lastInsertId().toInt();

    qDebug() << "healthCheckupId =" << healthCheckupId;

    // 插入健康检查项目
    foreach (const QJsonValue &item, HealthCheckupDate) {
        QJsonObject itemObject = item.toObject();
        QString itemName = itemObject["itemName"].toString();
        QString normalRange = itemObject["normalRange"].toString();
        QString inputData = itemObject["inputData"].toString();
        QString resultData = itemObject["resultField"].toString();
        QString responsiblePerson = itemObject["responsiblePerson"].toString();

        query.prepare("INSERT INTO health_checkup_items (health_checkup_id, item_name, normal_range, input_data, result_data, responsible_person) "
                      "VALUES (:healthCheckupId, :itemName, :normalRange, :inputData, :resultData, :responsiblePerson)");

        // 绑定健康检查项目的值
        query.bindValue(":healthCheckupId", healthCheckupId);
        query.bindValue(":itemName", itemName);
        query.bindValue(":normalRange", normalRange);
        query.bindValue(":inputData", inputData);
        query.bindValue(":resultData", resultData);
        query.bindValue(":responsiblePerson", responsiblePerson);

        if (!query.exec()) {
            qDebug() << "插入 health_checkup_items 时出错:" << query.lastError().text();
            db.rollback(); // 回滚事务
            return false; // 如果任何项目插入失败，返回失败
        }
    }

    db.commit(); // 提交事务
    return true; // 成功完成所有操作

}

bool DatabaseManager::UpdateHealthCheckData(QJsonObject &healthcheckupDate)
{
    QString patientName = healthcheckupDate["patientName"].toString();
    QString gender = healthcheckupDate["gender"].toString();
    QString birthDate = healthcheckupDate["birthDate"].toString();
    QString phoneNumber = healthcheckupDate["phoneNumber"].toString();
    QString packageName = healthcheckupDate["packageName"].toString();
    QString packageDate = healthcheckupDate["packageDate"].toString();
    QString status = healthcheckupDate["status"].toString();

    QJsonArray HealthCheckupDate = healthcheckupDate["HealthCheckupDate"].toArray();

    if (!isConnected()) {
        qDebug() << "数据库连接失败:" << db.lastError().text();
        return false;
    }

    // 开始事务
    db.transaction();

    // 更新主要健康检查信息
    QSqlQuery query(db);
    query.prepare("UPDATE health_checkup SET "
                  "gender = :gender, "
                  "birth_date = :birthDate, "
                  "phone_number = :phoneNumber, "
                  "package_name = :packageName, "
                  "package_date = :packageDate, "
                  "report_status = :reportStatus "
                  "WHERE patient_name = :patientName");

    query.bindValue(":patientName", patientName);
    query.bindValue(":gender", gender);
    query.bindValue(":birthDate", birthDate);
    query.bindValue(":phoneNumber", phoneNumber);
    query.bindValue(":packageName", packageName);
    query.bindValue(":packageDate", packageDate);
    query.bindValue(":reportStatus", status);

    // 执行查询
    if (!query.exec()) {
        qDebug() << "更新 health_checkup 时出错:" << query.lastError().text();
        db.rollback(); // 回滚事务
        return false;
    }

    // 获取健康检查 ID
    int healthCheckupId = getPatientIdByName(patientName); // 根据 patientName 获取 ID
    qDebug() << "healthCheckupId =" << healthCheckupId;

    // 更新健康检查项目
    foreach (const QJsonValue &item, HealthCheckupDate) {
        QJsonObject itemObject = item.toObject();
        QString itemName = itemObject["itemName"].toString();
        QString normalRange = itemObject["normalRange"].toString();
        QString inputData = itemObject["inputData"].toString();
        QString resultData = itemObject["resultField"].toString();
        QString responsiblePerson = itemObject["responsiblePerson"].toString();

        query.prepare("UPDATE health_checkup_items SET "
                      "normal_range = :normalRange, "
                      "input_data = :inputData, "
                      "result_data = :resultData, "
                      "responsible_person = :responsiblePerson "
                      "WHERE health_checkup_id = :healthCheckupId AND item_name = :itemName");

        // 绑定健康检查项目的值
        query.bindValue(":healthCheckupId", healthCheckupId);
        query.bindValue(":itemName", itemName);
        query.bindValue(":normalRange", normalRange);
        query.bindValue(":inputData", inputData);
        query.bindValue(":resultData", resultData);
        query.bindValue(":responsiblePerson", responsiblePerson);

        if (!query.exec()) {
            qDebug() << "更新 health_checkup_items 时出错:" << query.lastError().text();
            db.rollback(); // 回滚事务
            return false; // 如果任何项目更新失败，返回失败
        }
    }

    db.commit(); // 提交事务
    return true; // 成功完成所有操作

}


bool DatabaseManager::isExistCheckupDate(const QString &packageName, const QString &patientName, const QString &appointmentDate)
{
    qDebug() << "patientName=" << patientName;
    qDebug() << "packageName=" << packageName;
    qDebug() << "appointmentDate=" << appointmentDate;

    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return false; // 返回 -1 表示出错
    }

    QSqlQuery query(db);

    query.prepare("select count(*) from health_checkup "
                  "where package_name = :packageName "
                  "and patient_name = :patientName "
                  "and package_date = :appointmentDate");

    // 绑定参数
    query.bindValue(":packageName", packageName);
    query.bindValue(":patientName", patientName);
    query.bindValue(":appointmentDate", appointmentDate);

    // 执行查询
    if (query.exec())
    {
        if (query.next())
        {
            int count = query.value(0).toInt(); // 获取记录数
            if (count > 0)
            {
                // 存在记录
                return true;
            } else {
                // 不存在记录
                return  false;
            }
        }
    }
    else
    {
        // 查询执行失败的处理
        qDebug() << "查询执行失败: " << query.lastError().text();
        return false;
    }

}

bool DatabaseManager::updateStatusByAppointment(const int &patientId, const int &packageId, const QString &appointmentDate)
{
    if (!isConnected()) {
        qDebug() << "数据库连接失败:" << db.lastError().text();
        return false;
    }

    // 开始数据库事务
    if (!db.transaction()) {
        qDebug() << "事务开始失败:" << db.lastError().text();
        return false;
    }

    // 更新状态为“已体检”
    QSqlQuery query(db);
    query.prepare("UPDATE appointments SET status = '已体检' "
                  "WHERE user_id = :patientId AND package_id = :packageId AND appointment_date = :appointmentDate");

    query.bindValue(":patientId", patientId);
    query.bindValue(":packageId", packageId);
    query.bindValue(":appointmentDate", appointmentDate);

    // 执行更新查询
    if (!query.exec()) {
        qDebug() << "更新 appointments 时出错:" << query.lastError().text();
        db.rollback(); // 回滚事务
        return false;
    }

    // 提交事务
    if (!db.commit()) {
        qDebug() << "事务提交失败:" << db.lastError().text();
        return false;
    }

    return true; // 更新成功
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

QSqlQuery DatabaseManager::getPackageItemInfo(const QString &packagename)
{
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return QSqlQuery();
    }

    QSqlQuery query(db);

    query.prepare("SELECT hi.item_name, hi.normal_range  FROM  healthpackages hp  "
                  "JOIN  packageitemmapping pim ON hp.id = pim.package_id "
                  " JOIN  healthitems hi ON pim.item_id = hi.id "
                  " WHERE  hp.package_name = :packagename;");

    query.bindValue(":packagename",packagename);

    if (!query.exec()) {
        qDebug() << "Failed to retrieve appointment data:" << query.lastError().text();
        return QSqlQuery();  // 查询执行失败，返回空查询
    }
    return query;

}

QSqlQuery DatabaseManager::getRecordHealthCheckupDate(const QString &patientName)
{

    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return QSqlQuery();
    }

    QSqlQuery query(db);

    query.prepare("SELECT hci.item_name, hci.normal_range, hci.input_data,hci.result_data,hci.responsible_person "
                  "FROM health_checkup hc "
                  "JOIN health_checkup_items hci ON hc.id = hci.health_checkup_id "
                  "WHERE hc.patient_name = :patientName");
    query.bindValue(":patientName", patientName);

    if (!query.exec()) {
        qDebug() << "Failed to retrieve appointment data:" << query.lastError().text();
        return QSqlQuery();  // 查询执行失败，返回空查询
    }
    return query;
}

QSqlQuery DatabaseManager::getAppointmentInfoByusername(const QString &username)
{
    int userId = getUserIdByUsername(username);

    qDebug() << "userId=" << userId;
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return QSqlQuery();  // 返回空查询
    }

    QSqlQuery query(db);
    query.prepare("SELECT hp.package_name, a.appointment_date, a.status "
                  "FROM appointments a "
                  "JOIN healthpackages hp ON a.package_id = hp.id "
                  "WHERE a.user_id = :userId");
    query.bindValue(":userId", userId);

    if (!query.exec()) {
        qDebug() << "Failed to retrieve appointment data:" << query.lastError().text();
        return QSqlQuery();  // 查询执行失败，返回空查询
    }

    return query;  // 返回查询结果
}

QSqlQuery DatabaseManager::getPatientCheckupDate(const QString &patientName, const QString &packageName, const QString &appointmentDate)
{
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return QSqlQuery();
    }

    QSqlQuery query(db);
    query.prepare("SELECT * FROM health_checkup "
                  "WHERE patient_name = :patientName "
                  "AND package_name = :packageName "
                  "AND package_date = :appointmentDate");

    // 绑定参数
    QString cleanPatientName = patientName.trimmed();
    QString cleanPackageName = packageName.trimmed();
    query.bindValue(":patientName", cleanPatientName);
    query.bindValue(":packageName", cleanPackageName);
    query.bindValue(":appointmentDate", appointmentDate);


    if (!query.exec()) {
        qDebug() << "Query execution failed:" << query.lastError().text();
        return QSqlQuery();
    }

    return query;
}

bool DatabaseManager::updatePatientReport(const QJsonObject &reportDateObj)
{
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return false;
    }

    QString patientName = reportDateObj["patientName"].toString();
    QString packageName = reportDateObj["packageName"].toString();
    QString packageDate = reportDateObj["packageDate"].toString();
    QString doctorAdvice = reportDateObj["doctorAdvice"].toString();
    QString editDoctor = reportDateObj["editDorctor"].toString();  // 修正拼写错误
    QString reportGenerateTime = reportDateObj["reportGenerateTime"].toString();   ;

    QSqlQuery query(db);
    query.prepare(
                "UPDATE health_checkup "
                "SET doctor_advice = :doctorAdvice, doctor_name = :editDoctor, report_generated = :reportGenerateTime, report_status = '已审核' "
                "WHERE patient_name = :patientName AND package_name = :packageName AND package_date = :packageDate AND report_status = '待审核';"
                );


    query.bindValue(":doctorAdvice", doctorAdvice);
    query.bindValue(":editDoctor", editDoctor);  // 修改为 editDoctor
    query.bindValue(":reportGenerateTime", reportGenerateTime);
    query.bindValue(":patientName", patientName);
    query.bindValue(":packageName", packageName);
    query.bindValue(":packageDate", packageDate);


    if (query.exec()) {
        qDebug() << "Record updated successfully.";
        return true;
    } else {
        qDebug() << "Error updating record: " << query.lastError().text();
        return false;
    }


}


QList<QVariantMap> DatabaseManager::GetHealthExaminationPatientInfo(const int &doctorId)
{
    //查询体检病人信息
    QList<QVariantMap> checkupList;

    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return checkupList;
    }

    // 准备 SQL 查询以获取健康检查信息
    QString sql = R"(
           SELECT
               hc.id AS health_checkup_id,
               hc.patient_name,
               hc.gender,
               hc.birth_date,
               hc.phone_number,
               hc.package_name,
               hc.report_generated,
               hc.report_status,
               a.appointment_date
           FROM
               appointments a
           JOIN
               users u ON a.user_id = u.id
           JOIN
               health_checkup hc ON hc.patient_name = u.username
                                 AND hc.package_name = (SELECT  package_name FROM healthitems p WHERE p.id = a.package_id)
                                 AND hc.package_date = a.appointment_date
           WHERE
               a.doctor_id = :doctorId
               AND a.status = '已体检'
               AND hc.report_status = '待审核'
       )";
    // 执行查询
    QSqlQuery checkUpInfoQuery(db);
    checkUpInfoQuery.prepare(sql);
    checkUpInfoQuery.bindValue(":doctorId", doctorId);

    if (!checkUpInfoQuery.exec()) {
        qDebug() << "Error executing query:" << checkUpInfoQuery.lastError().text();
        return checkupList;
    }

    // 遍历查询结果并填充数据
    while (checkUpInfoQuery.next()) {
        QVariantMap checkupRecord;
        checkupRecord["health_checkup_id"] = checkUpInfoQuery.value("health_checkup_id");
        checkupRecord["patient_name"] = checkUpInfoQuery.value("patient_name");
        checkupRecord["gender"] = checkUpInfoQuery.value("gender");
        checkupRecord["birth_date"] = checkUpInfoQuery.value("birth_date");
        checkupRecord["phone_number"] = checkUpInfoQuery.value("phone_number");
        checkupRecord["package_name"] = checkUpInfoQuery.value("package_name");
        checkupRecord["report_generated"] = checkUpInfoQuery.value("report_generated");
        checkupRecord["report_status"] = checkUpInfoQuery.value("report_status");
        checkupRecord["appointment_date"] = checkUpInfoQuery.value("appointment_date");

        // 添加到列表
        checkupList.append(checkupRecord);
    }

    return checkupList;
}

QList<QVariantMap> DatabaseManager::GetPatientHealthExaminationData(const int &healthCheckupId)
{
    //查询病人体检数据
    QList<QVariantMap> checkupItemsList;

    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return checkupItemsList;
    }

    QString sql = R"(
           SELECT
               hci.id AS health_checkup_item_id,
               hci.health_checkup_id,
               hci.item_name,
               hci.normal_range,
               hci.input_data,
               hci.result_data,
               hci.responsible_person
           FROM
               health_checkup_items hci
           WHERE
               hci.health_checkup_id = :healthCheckupId
       )";

    QSqlQuery checkupItemsQuery(db);
    checkupItemsQuery.prepare(sql);
    checkupItemsQuery.bindValue(":healthCheckupId", healthCheckupId);

    if (!checkupItemsQuery.exec()) {
        qDebug() << "Error executing checkup items query for healthCheckupId:"
                 << healthCheckupId << " - " << checkupItemsQuery.lastError().text();
        return checkupItemsList; // 返回空列表
    }

    // 遍历查询结果并构建每个健康检查项目的对象
    while (checkupItemsQuery.next()) {
        QVariantMap itemObject;
        itemObject["health_checkup_item_id"] = checkupItemsQuery.value("health_checkup_item_id").toInt();
        itemObject["health_checkup_id"] = checkupItemsQuery.value("health_checkup_id").toInt();
        itemObject["item_name"] = checkupItemsQuery.value("item_name").toString();
        itemObject["normal_range"] = checkupItemsQuery.value("normal_range").toString();
        itemObject["input_data"] = checkupItemsQuery.value("input_data").toString();
        itemObject["result_data"] = checkupItemsQuery.value("result_data").toString();
        itemObject["responsible_person"] = checkupItemsQuery.value("responsible_person").toString();

        // 将项目对象添加到列表
        checkupItemsList.append(itemObject);
    }

    return checkupItemsList;
}


bool DatabaseManager::CancelAppointment(const QString &username, const QString &packageName, const QString &appointmentDate)
{


    int userId = getUserIdByUsername(username);
    int packageId =getPackageIdByName(packageName);

    qDebug()<<"userId="<<userId;
    qDebug()<<"packageId="<<packageId;

    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return false;
    }

    qDebug()<<"appointmentDate="<<appointmentDate;
    // 准备 SQL 语句
    QSqlQuery query(db);
    query.prepare("DELETE FROM appointments "
                  "WHERE user_id = :userId AND package_id = :packageId AND appointment_date = :appointmentDate");

    // 绑定参数
    query.bindValue(":userId", userId);
    query.bindValue(":packageId", packageId);
    query.bindValue(":appointmentDate", appointmentDate);

    // 执行删除
    if (!query.exec()) {
        qDebug() << "Failed to delete appointment:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::updateReportPath(const QString &patientName, const QString &packageName, const QString &appointmentDate, const QString &reportPath)
{
    if (!isConnected()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
        return false;
    }


    QSqlQuery query(db);
    query.prepare(
                "UPDATE health_checkup "
                "SET report_path = :reportPath, report_status = '已完成'"
                "WHERE patient_name = :patientName AND package_name = :packageName AND package_date = :packageDate And report_status = '已审核';"
                );

    query.bindValue(":reportPath", reportPath);
    query.bindValue(":patientName", patientName);
    query.bindValue(":packageName", packageName);
    query.bindValue(":packageDate", appointmentDate);

    if (query.exec()) {
        qDebug() << "Record updated successfully.";
        return true;
    } else {
        qDebug() << "Error updating record: " << query.lastError().text();
        return false;
    }

}

bool DatabaseManager::updateAppointMents(const QString &patientName, const QString &packageName, const QString &appointmentDate)
{
    if (!isConnected()) {
        qDebug() << "数据库连接失败：" << db.lastError().text();
        return -1; // 连接失败返回 -1 表示未找到
    }

    QSqlQuery query(db);

    int userId =getUserIdByUsername(patientName);
    int packageId =getPackageIdByName(packageName);

    if(userId== -1 || packageId ==-1)
    {
        return false;
    }

    query.prepare("UPDATE appointments SET status = '已完成' WHERE user_id = :userId "
                  "AND package_id = :packageId "
                  "AND appointment_date = :appointmentDate");

    query.bindValue(":userId", userId);
    query.bindValue(":packageId", packageId);
    query.bindValue(":appointmentDate", appointmentDate);


    if (!query.exec()) {
        qDebug() << "更新失败：" << query.lastError().text();
        return false; // Return false if update fails
    }

    return true; // Return true on successful update

}



int DatabaseManager::getPatientIdByName(const QString &patientName)
{
    if (!isConnected()) {
        qDebug() << "数据库连接失败：" << db.lastError().text();
        return -1; // 连接失败返回 -1 表示未找到
    }

    QSqlQuery query(db);
    query.prepare("SELECT id FROM health_checkup WHERE patient_name = :patientName");
    query.bindValue(":patientName", patientName);

    if (!query.exec()) {
        qDebug() << "查询 health_checkup 时出错：" << query.lastError().text();
        return -1; // 查询失败返回 -1
    }

    if (query.next()) {
        return query.value(0).toInt(); // 返回查询到的 id
    } else {
        qDebug() << "未找到患者：" << patientName;
        return -1; // 未找到患者返回 -1
    }
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
