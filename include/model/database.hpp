#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <QtCore/QVariant>
#include <QString>
#include "UserData.hpp"
#include "MachineStatus.hpp"
#include "CroplandInfo.hpp"
#include <QFile>
class Database
{
public:
    // 获取单例数据库实例
    static Database& instance(const QString &dbName = "");

    // 禁止拷贝构造和赋值操作
    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;

    // 检查数据库中是否已存在指定的表
    bool isTableExist(const QString &tableName);
    // 初始化创建/打开一个数据库文件
    void dbInit(const QString &dbName);

    //--------------------------------------------------------------创
    // 创建UserData表
    void createUserTable();
    // 创建MachineStatus表
    void createMachineStatusTable();
    // 创建CroplandInfo表
    void createCroplandInfoTable();
    //--------------------------------------------------------------创

    //--------------------------------------------------------------增
    bool addUserData_NewUser(const QString &username, const QString &password);
    bool addMachineStatus(MachineStatus &machine);
    bool updateCroplandInfo(CroplandInfo &cropland);
    //--------------------------------------------------------------增
    //--------------------------------------------------------------查
    QString queryPasswordByUsername(const QString &username);
private:
    // 私有构造函数
    Database(const QString &dbName);

    // 执行 SQL 语句
    void executeQuery(const QString &queryStr);

    // 数据库实例
    QSqlDatabase db;
};

#endif // DATABASE_HPP
