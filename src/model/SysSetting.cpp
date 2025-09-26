#include "SysSetting.hpp"
#include <QDateTime>

// 静态实例化单例
SysSetting& SysSetting::instance() {
    static SysSetting instance;  // 局部静态变量，确保线程安全且只初始化一次
    return instance;
}

// 私有构造函数
SysSetting::SysSetting() :
    gnssStatus_(false),
    cat1SensorStatus_(false),
    rosConnectionStatus_(false),
    processStatus_(false) {
    // 初始化系统时间为当前时间
    sysTime_ = QDateTime::currentDateTime().toString();
}

/*
 * 设置GNSS状态
 */
void SysSetting::setGNSSStatus(bool status) {
    gnssStatus_ = status;  // 更新GNSS状态
}

/*
 * 设置4G传感器状态
 */
void SysSetting::set4GSensorStatus(bool status) {
    cat1SensorStatus_ = status;  // 更新4G传感器状态
}

/*
 * 设置ROS连接状态
 */
void SysSetting::setRosConnectionStatus(bool status) {
    rosConnectionStatus_ = status;  // 更新ROS连接状态
}

/*
 * 设置进程状态
 */
void SysSetting::setProcessStatus(bool status) {
    processStatus_ = status;  // 更新进程状态
}

/*
 * 设置系统时间
 */
void SysSetting::setSysTime(const QString& time) {
    sysTime_ = time;  // 更新系统时间
}

// 设置gnssLaunchStatus_的状态
void SysSetting::setGNSSLaunchState(bool state)
{
    gnssLaunchState_ = state;
}

/*
 * 获取GNSS状态
 */
bool SysSetting::getGNSSStatus() const {
    return gnssStatus_;  // 返回GNSS状态
}

/*
 * 获取4G传感器状态
 */
bool SysSetting::get4GSensorStatus() const {
    return cat1SensorStatus_;  // 返回4G传感器状态
}

/*
 * 获取ROS连接状态
 */
bool SysSetting::getRosConnectionStatus() const {
    return rosConnectionStatus_;  // 返回ROS连接状态
}

/*
 * 获取进程状态
 */
bool SysSetting::getProcessStatus() const {
    return processStatus_;  // 返回进程状态
}

/*
 * 获取系统时间
 */
QString SysSetting::getSysTime() const {
    return sysTime_;  // 返回系统时间
}

// 获取gnssLaunchStatus_的状态值
bool SysSetting::getGNSSLaunchState() const
{
    return gnssLaunchState_;
}
