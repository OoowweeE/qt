#include "UserData.hpp"
//#include <QtCrypto>

// 静态实例化单例
UserData& UserData::instance() {
    static UserData instance;  // 局部静态变量，确保线程安全且只初始化一次
    return instance;
}

// 私有构造函数
UserData::UserData() : 
    userId_(0), 
    isActive_(true) 
{
    // 初始化密码为空字符串或进行加密处理
    password_ = "";
}

/*
 * 设置用户标识符
 */
void UserData::setUserId(int userId) {
    userId_ = userId;  // 更新用户标识符
}

/*
 * 设置用户名
 */
void UserData::setUsername(const QString& username) {
    username_ = username;  // 更新用户名
}

/*
 * 设置密码（存储时应该加密）
 */
void UserData::setPassword(const QString& password) {
    // 使用加密函数来存储密码
    password_ = password;
}

/*
 * 设置用户权限
 */
void UserData::setUserRole(const QString& role) {
    userRole_ = role;  // 更新用户权限
}

/*
 * 设置用户激活状态
 */
void UserData::setActive(bool active) {
    isActive_ = active;  // 更新用户激活状态
}

/*
 * 设置用户偏好语言
 */
void UserData::setLanguagePreference(const QString& language) {
    languagePreference_ = language;  // 更新用户偏好语言
}

/*
 * 设置用户主题
 */
void UserData::setThemePreference(const QString& theme) {
    themePreference_ = theme;  // 更新用户主题
}

/*
 * 获取用户标识符
 */
int UserData::getUserId() const {
    return userId_;  // 返回用户标识符
}

/*
 * 获取用户名
 */
QString UserData::getUsername() const {
    return username_;  // 返回用户名
}

/*
 * 获取密码（返回加密后的密码）
 */
QString UserData::getPassword() const {
    return password_;  // 返回加密后的密码
}

/*
 * 获取用户权限
 */
QString UserData::getUserRole() const {
    return userRole_;  // 返回用户权限
}

/*
 * 获取用户激活状态
 */
bool UserData::isActive() const {
    return isActive_;  // 返回用户激活状态
}

/*
 * 获取用户偏好语言
 */
QString UserData::getLanguagePreference() const {
    return languagePreference_;  // 返回用户偏好语言
}

/*
 * 获取用户主题
 */
QString UserData::getThemePreference() const {
    return themePreference_;  // 返回用户主题
}
