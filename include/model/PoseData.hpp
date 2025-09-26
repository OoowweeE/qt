#ifndef POSEDATA_H
#define POSEDATA_H

#include <QString>
#include <QDebug>
#include <QtMath>

class PoseData
{
public:
    // 获取单例实例
    static PoseData& instance();

    // 禁止复制构造和赋值
    PoseData(const PoseData&) = delete;
    PoseData& operator=(const PoseData&) = delete;

    // update——存储/更新农机传感器数据
    // 更新GNSS数据，包括经纬度、高程，可选更新卫星数量
    void updateGNSSData(double latitude, double longitude, double altitude, int satelliteNum = -1);
    // 更新IMU数据，包括俯仰角、横滚角和航向角
    void updateIMUData(double pitch, double roll, double yaw);
    // 更新速度数据，包括X轴速度和Y轴速度，并计算线速度
    void updateSpeed(double speedX, double speedY);
    // 更新二维位姿数据，包括x和y坐标
    void updateXYPose(double x, double y);
    // 更新农机车头角度
    void updateRotation(double rotation);
    // 更新运动偏差
    void updateBias(double bias);

    // get——获取数据
    // 获取所有位姿数据，返回格式化字符串
    QString getPoseData() const;
    // 获取GPS数据，包括经度、纬度和高程，返回格式化字符串
    QString getGpsData() const;
    // 获取单独的经度数据，返回字符串
    QString getGpsData_latitude() const;
    // 获取单独的纬度数据，返回字符串
    QString getGpsData_longitude() const;
    // 获取高程数据，返回字符串
    QString getGpsData_altitude() const;
    // 获取卫星数量，返回字符串
    QString getGpsData_satelliteNum() const;
    // 获取IMU数据，返回格式化字符串
    QString getIMUData() const;
    // 获取俯仰角，返回字符串
    QString getIMUData_pitch() const;
    // 获取横滚角，返回字符串
    QString getIMUData_roll() const;
    // 获取航向角，返回字符串
    QString getIMUData_yaw() const;
    // 获取X轴速度，返回字符串
    QString getSpeed_X() const;
    // 获取Y轴速度，返回字符串
    QString getSpeed_Y() const;
    // 获取线速度，返回字符串
    QString getSpeed() const;
    // 获取农机车头角度，返回字符串
    QString getRotation() const;
    // 获取运动偏差，返回字符串
    QString getBias() const;




private:
    // 私有构造函数，确保只有一个实例
    PoseData();

    // 存储导航位姿传感器数据
    // 1 经纬度、高程
    double latitude_;
    double longitude_;
    double altitude_;
    int satelliteNum_;//卫星数量
    // 2 XYZ三轴角度
    double pitch_;
    double roll_;
    double yaw_;
    // 3 X、Y轴速度和线速度
    double speedX_;
    double speedY_;
    double speed_;
    // 4 二维x、y坐标
    double x_;
    double y_;
    // 5 农机车头角度
    double rotation_;
    // 6 偏差
    double bias_;

};

#endif // POSEDATA_H
