#ifndef USERDATA_H
#define USERDATA_H

#include <QString>

class UserData
{
public:
    // 获取单例实例
    static UserData& instance();

    // 禁止复制构造和赋值
    UserData(const UserData&) = delete;
    UserData& operator=(const UserData&) = delete;

    // Setters
    void setUserId(int userId); // 设置用户标识符
    void setUsername(const QString& username); // 设置用户名
    void setPassword(const QString& password); // 设置密码（存储时应该加密）
    void setUserRole(const QString& role); // 设置用户权限
    void setActive(bool active); // 设置用户激活状态
    void setLanguagePreference(const QString& language); // 设置用户偏好语言
    void setThemePreference(const QString& theme); // 设置用户主题

    // Getters
    int getUserId() const; // 获取用户标识符
    QString getUsername() const; // 获取用户名
    QString getPassword() const; // 获取密码（返回加密后的密码）
    QString getUserRole() const; // 获取用户权限
    bool isActive() const; // 获取用户激活状态
    QString getLanguagePreference() const; // 获取用户偏好语言
    QString getThemePreference() const; // 获取用户主题

private:
    // 私有构造函数，确保只有一个实例
    UserData();

    // 用户数据成员
    int userId_;  // 用户标识符
    QString username_;  // 用户名
    QString password_;  // 密码（应加密存储）
    QString userRole_;  // 用户权限
    bool isActive_;  // 用户激活状态
    QString languagePreference_;  // 用户偏好语言
    QString themePreference_;  // 用户主题
};

#endif // USERDATA_H
