#ifndef SYSSETTING_H
#define SYSSETTING_H

#include <QString>

class SysSetting
{
public:
    // 获取单例实例
    static SysSetting& instance();

    // 禁止复制构造和赋值
    SysSetting(const SysSetting&) = delete;
    SysSetting& operator=(const SysSetting&) = delete;


    bool gnssLaunchState_;      // GNSS转换状态(用来判断是否启动了launch文件）
    bool loadPathLaunchState_;  // 路径读取的状态（1为已读，0为未读或清空）
    bool savePathLaunchState_;  // 路径记录launch文件的状态（1为开始记录，0为停止/未记录）
    bool pathPersuitState_;     // 路径跟踪launch文件的状态（1为开始跟踪了，0为停止）
    bool harvestCANLaunchState_;// 产量监测CAN转换状态（用来判断是否启动launch文件，读取can数据）
    //bool


    // Setters
    void setGNSSStatus(bool status); // 设置GNSS状态
    void set4GSensorStatus(bool status); // 设置4G传感器状态
    void setRosConnectionStatus(bool status); // 设置ROS连接状态
    void setProcessStatus(bool status); // 设置进程状态
    void setSysTime(const QString& time); // 设置系统时间

    void setGNSSLaunchState(bool state); //设置GNSS转换状态(用来判断是否启动了launch文件）

    // Getters
    bool getGNSSStatus() const; // 获取GNSS状态
    bool get4GSensorStatus() const; // 获取4G传感器状态
    bool getRosConnectionStatus() const; // 获取ROS连接状态
    bool getProcessStatus() const; // 获取进程状态
    QString getSysTime() const; // 获取系统时间

    bool getGNSSLaunchState() const; //获取GNSS转换状态(用来判断是否启动了launch文件）

    //bool

private:
    // 私有构造函数，确保只有一个实例
    SysSetting();

    // 系统设置数据成员
    bool gnssStatus_;  // GNSS状态
    bool cat1SensorStatus_;  // 4G传感器状态
    bool rosConnectionStatus_;  // ROS连接状态
    bool processStatus_;  // 进程状态
    QString sysTime_;  // 系统时间


};

#endif // SYSSETTING_H
