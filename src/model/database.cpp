#include "database.hpp"
#include <QDebug>

// 获取单例数据库实例
Database& Database::instance(const QString &dbName) {
    static Database instance(dbName);  // 局部静态变量，线程安全
    return instance;
}

bool Database::isTableExist(const QString &tableName)
{
    QSqlQuery query;
    query.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name=:tableName");
    query.bindValue(":tableName", tableName);
    if (query.exec() && query.next()) {
        return true;  // 表存在
    }
    return false;  // 表不存在
}

void Database::dbInit(const QString &dbName)
{
    if (QFile::exists(dbName)) {
        // 如果数据库文件存在，就直接打开数据库
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dbName);
        if (!db.open()) {
            qDebug() << "Failed to open database:" << db.lastError().text();
        } else {
            qDebug() << dbName <<"Database opened successfully!(已读取本地数据库)";
        }
    } else {
        // 如果数据库文件不存在，则创建新的数据库文件
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dbName);
        if (!db.open()) {
            qDebug() << "Failed to open database:" << db.lastError().text();
        } else {
            qDebug() << "Database created and opened successfully!（已新建本地数据库）";
        }

        // 创建数据表
        createUserTable();
        createMachineStatusTable();
    }
}

// 私有构造函数
Database::Database(const QString &dbName) {
    dbInit(dbName);
}

void Database::executeQuery(const QString &queryStr)
{
    QSqlQuery query;
    if (!query.exec(queryStr)) {
        qDebug() << "Query failed:" << query.lastError();
    }
}

void Database::createUserTable() {
    QString queryStr = "CREATE TABLE IF NOT EXISTS UserData ("
                       "userId INTEGER PRIMARY KEY AUTOINCREMENT, "  // AUTOINCREMENT 确保每次插入时 ID 自动增加
                       "username TEXT NOT NULL, "
                       "password TEXT NOT NULL, "
                       "userRole TEXT, "
                       "isActive BOOLEAN, "
                       "languagePreference TEXT, "
                       "themePreference TEXT)";
    executeQuery(queryStr);
}

void Database::createMachineStatusTable() {
    QString queryStr = "CREATE TABLE IF NOT EXISTS MachineStatus ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT, "        // 添加主键，自动递增
                       "tKind TEXT NOT NULL, "                         // 机器类型
                       "tId TEXT NOT NULL, "                           // 机器ID
                       "tCutWidth REAL, "                              // 切割宽度
                       "tMinTurningRadius REAL, "                      // 最小转弯半径
                       "tMainAntennaHeight REAL, "                     // 主天线高度
                       "tAntennaToAxisDistance REAL, "                 // 天线到轴的距离
                       "tAntennaToFrontAxleDistance REAL, "            // 天线到前轴的距离
                       "tWheelbase REAL, "                             // 轴距
                       "tFrontWheel REAL, "                            // 前轮半径
                       "tSpeed REAL, "                                 // 速度
                       "tMainSubAntennaDistance REAL)";                // 主副天线的距离
    executeQuery(queryStr);
}

void Database::createCroplandInfoTable()
{
    QString queryStr = "CREATE TABLE IF NOT EXISTS CroplandInfo ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT, "   // 自动递增主键
                       "driver TEXT, "                            // 作业人
                       "fieldName TEXT, "                         // 地块名
                       "fieldArea REAL, "                         // 地块面积
                       "cropVariety TEXT, "                       // 作物品种
                       "workType TEXT, "                          // 作业类型
                       "weather TEXT, "                           // 天气状况
                       "soilType TEXT, "                          // 土壤类型
                       "workDate TEXT)";                          // 作业日期
    executeQuery(queryStr);
}


bool Database::addUserData_NewUser(const QString &username, const QString &password)
{
    // 1. 获取当前最大的 userId
    QString queryStr = "SELECT MAX(userId) FROM UserData";
    QSqlQuery query;
    if (!query.exec(queryStr)) {
        qDebug() << "Failed to get max userId:" << query.lastError();
        return false;  // 查询失败
    }

    int newUserId = 1;  // 默认新用户的 userId 从 1 开始
    if (query.next()) {
        newUserId = query.value(0).toInt() + 1;  // 获取当前最大 userId 并自增
    }

    // 2. 构建插入语句
    query.prepare("INSERT INTO UserData (userId, username, password, userRole, isActive, languagePreference, themePreference) "
                  "VALUES (:userId, :username, :password, :userRole, :isActive, :languagePreference, :themePreference)");
    query.bindValue(":userId", newUserId);
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.bindValue(":userRole", "user");  // 默认用户角色为 "user"
    query.bindValue(":isActive", 1);  // 默认启用用户
    query.bindValue(":languagePreference", 1);  // 默认语言设置为1
    query.bindValue(":themePreference",1);  // 默认主题设置为1

    // 3. 执行插入操作
    if (!query.exec()) {
        qDebug() << "Failed to insert user:" << query.lastError();
        return false;  // 插入失败
    }

    qDebug() << "User registered successfully!";
    return true;
}

bool Database::addMachineStatus(MachineStatus &machine)
{
    // 先检查是否存在 CroplandInfo 表，如果不存在则创建
    if (!isTableExist("MachineStatus")) {
        createMachineStatusTable();  // 如果表不存在，则创建表
    }
    // 获取机器状态信息
    QString tKind = machine.tKind_;
    QString tId = machine.tId_;
    double tCutWidth = machine.tCutWidth_;
    double tMinTurningRadius = machine.tMinTurningRadius_;
    double tMainAntennaHeight = machine.tMainAntennaHeight_;
    double tAntennaToAxisDistance = machine.tAntennaToAxisDistance_;
    double tAntennaToFrontAxleDistance = machine.tAntennaToFrontAxleDistance_;
    double tWheelbase = machine.tWheelbase_;
    double tFrontWheel = machine.tFrontWheel_;
    double tSpeed = machine.tSpeed_;  // 添加速度字段
    double tMainSubAntennaDistance = machine.tMainSubAntennaDistance_; // 添加主副天线的距离字段

    // 创建 SQL 插入语句
    QSqlQuery query;

    // 绑定参数并执行 SQL 插入
    query.prepare("INSERT INTO MachineStatus (tKind, tId, tCutWidth, tMinTurningRadius, "
                  "tMainAntennaHeight, tAntennaToAxisDistance, tAntennaToFrontAxleDistance, "
                  "tWheelbase, tFrontWheel, tSpeed, tMainSubAntennaDistance) "
                  "VALUES (:tKind, :tId, :tCutWidth, :tMinTurningRadius, "
                  ":tMainAntennaHeight, :tAntennaToAxisDistance, :tAntennaToFrontAxleDistance, "
                  ":tWheelbase, :tFrontWheel, :tSpeed, :tMainSubAntennaDistance)");

    // 绑定字段值
    query.bindValue(":tKind", tKind);
    query.bindValue(":tId", tId);
    query.bindValue(":tCutWidth", tCutWidth);
    query.bindValue(":tMinTurningRadius", tMinTurningRadius);
    query.bindValue(":tMainAntennaHeight", tMainAntennaHeight);
    query.bindValue(":tAntennaToAxisDistance", tAntennaToAxisDistance);
    query.bindValue(":tAntennaToFrontAxleDistance", tAntennaToFrontAxleDistance);
    query.bindValue(":tWheelbase", tWheelbase);
    query.bindValue(":tFrontWheel", tFrontWheel);
    query.bindValue(":tSpeed", tSpeed);  // 绑定速度字段
    query.bindValue(":tMainSubAntennaDistance", tMainSubAntennaDistance);  // 绑定主副天线的距离字段

    // 执行查询
    if (!query.exec()) {
        qDebug() << "Failed to insert machine status:" << query.lastError().text();
        return false;
    }

    // 返回插入成功
    return true;
}

bool Database::updateCroplandInfo(CroplandInfo &cropland)
{
    // 先检查是否存在 CroplandInfo 表，如果不存在则创建
    if (!isTableExist("CroplandInfo")) {
        createCroplandInfoTable();  // 如果表不存在，则创建表
    }

    // 获取作业信息
    QString driver = cropland.driver_;
    QString fieldName = cropland.fieldName_;
    double fieldArea = cropland.fieldArea_;
    QString cropVariety = cropland.cropVariety_;
    QString workType = cropland.workType_;
    QString weather = cropland.weather_;
    QString soilType = cropland.soilType_;
    QString workDate = cropland.workDate_;

    // 创建 SQL 插入语句（不再使用 WHERE id）
    QSqlQuery query;

    query.prepare("INSERT INTO CroplandInfo (driver, fieldName, fieldArea, cropVariety, "
                  "workType, weather, soilType, workDate) "
                  "VALUES (:driver, :fieldName, :fieldArea, :cropVariety, "
                  ":workType, :weather, :soilType, :workDate)");

    // 绑定字段值
    query.bindValue(":driver", driver);
    query.bindValue(":fieldName", fieldName);
    query.bindValue(":fieldArea", fieldArea);
    query.bindValue(":cropVariety", cropVariety);
    query.bindValue(":workType", workType);
    query.bindValue(":weather", weather);
    query.bindValue(":soilType", soilType);
    query.bindValue(":workDate", workDate);

    // 执行查询
    if (!query.exec()) {
        qDebug() << "Failed to insert cropland info:" << query.lastError().text();
        return false;
    }

    // 返回插入成功
    return true;
}




//--------------------------------------------------------------------------------------------------------------------------------查
// 查询UserData表中的密码，通过用户名查询
QString Database::queryPasswordByUsername(const QString &username)
{
    QSqlQuery query;
    query.prepare("SELECT password FROM UserData WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "Failed to query password:" << query.lastError();
        return "";  // 返回空字符串表示查询失败
    }

    if (query.next()) {
        return query.value(0).toString();  // 返回查询到的密码
    }

    return "";  // 用户名不存在，返回空字符串
}





