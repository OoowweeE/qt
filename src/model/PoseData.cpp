#include "PoseData.hpp"

// 静态实例化单例
PoseData& PoseData::instance() {
    static PoseData instance;  // 局部静态变量，确保线程安全且只初始化一次
    return instance;
}

// 私有构造函数
PoseData::PoseData() {
    // 1 经纬度、高程、卫星数
    latitude_ = 0.0;        // 初始化经度
    longitude_ = 0.0;       // 初始化纬度
    altitude_ = 0.0;        // 初始化高程
    satelliteNum_ = 0;      // 初始化卫星数量

    // 2 XYZ三轴角度
    pitch_ = 0.0;           // 初始化俯仰角
    roll_ = 0.0;            // 初始化横滚角
    yaw_ = 0.0;             // 初始化航向角

    // 3 X、Y轴速度和线速度
    speedX_ = 0.0;          // 初始化X轴速度
    speedY_ = 0.0;          // 初始化Y轴速度
    speed_ = 0.0;           // 初始化线速度

    // 4 二维x、y坐标
    x_ = 0.0;               // 初始化二维x坐标
    y_ = 0.0;               // 初始化二维y坐标

    // 5 农机车头角度
    rotation_ = 0.0;        // 初始化农机车头角度

    // 6 偏差
    bias_ = 0.0;            // 初始化偏差
}

/*@
 * update
 * 更新GNSS数据，包括经纬度、高程和卫星数量
 * @*/
void PoseData::updateGNSSData(double latitude, double longitude, double altitude, int satelliteNum) {
    latitude_ = latitude;   // 更新经度
    longitude_ = longitude; // 更新纬度
    altitude_ = altitude;   // 更新高程

    // 如果提供了satelliteNum参数，并且satelliteNum不是默认值，那么更新卫星数量
    if (satelliteNum != -1) {
        satelliteNum_ = satelliteNum; // 更新卫星数量
    }
}

/*
 * 更新IMU数据，包括俯仰角、横滚角和航向角
 */
void PoseData::updateIMUData(double pitch, double roll, double yaw) {
    pitch_ = pitch; // 更新俯仰角
    roll_ = roll;   // 更新横滚角
    yaw_ = yaw;     // 更新航向角
}

/*
 * 更新速度，包括X轴速度和Y轴速度，并计算线速度
 */
void PoseData::updateSpeed(double speedX, double speedY) {
    speedX_ = speedX;                  // 更新X轴速度
    speedY_ = speedY;                  // 更新Y轴速度
    speed_ = qSqrt(speedX * 2 + speedY * 2); // 根据速度平方和计算线速度
}

/*
 * 更新二维x、y坐标
 */
void PoseData::updateXYPose(double x, double y) {
    x_ = x; // 更新二维x坐标
    y_ = y; // 更新二维y坐标
}

/*
 * 更新农机车头角度
 */
void PoseData::updateRotation(double rotation) {
    rotation_ = rotation; // 更新农机车头角度
}

/*
 * 更新运动偏差
 */
void PoseData::updateBias(double bias) {
    bias_ = bias; // 更新偏差
}


/*
 * 获取所有位姿数据，返回格式化字符串
 */
QString PoseData::getPoseData() const {
    return QString("Latitude: %1, Longitude: %2, Altitude: %3, Satellites: %4, Pitch: %5, Roll: %6, Yaw: %7, SpeedX: %8, SpeedY: %9, Speed: %10, X: %11, Y: %12, Rotation: %13, Bias: %14")
        .arg(latitude_)
        .arg(longitude_)
        .arg(altitude_)
        .arg(satelliteNum_)
        .arg(pitch_)
        .arg(roll_)
        .arg(yaw_)
        .arg(speedX_)
        .arg(speedY_)
        .arg(speed_)
        .arg(x_)
        .arg(y_)
        .arg(rotation_)
        .arg(bias_);
}
/*
 * 获取GPS数据，包括经度、纬度和高程，返回格式化字符串
 */
QString PoseData::getGpsData() const {
    return QString("Latitude: %1, Longitude: %2, altitude：%3").arg(latitude_).arg(longitude_).arg(altitude_);
}

// 获取单独的经度数据，返回字符串
QString PoseData::getGpsData_latitude() const {
    return QString::number(latitude_);
}

// 获取单独的纬度数据，返回字符串
QString PoseData::getGpsData_longitude() const {
    return QString::number(longitude_);
}

// 获取高程数据，返回字符串
QString PoseData::getGpsData_altitude() const {
    return QString::number(altitude_);
}

// 获取卫星数量，返回字符串
QString PoseData::getGpsData_satelliteNum() const {
    return QString::number(satelliteNum_);
}

// 获取俯仰角，返回字符串
QString PoseData::getIMUData_pitch() const {
    return QString::number(pitch_);
}

// 获取横滚角，返回字符串
QString PoseData::getIMUData_roll() const {
    return QString::number(roll_);
}

// 获取航向角，返回字符串
QString PoseData::getIMUData_yaw() const {
    return QString::number(yaw_);
}

// 获取X轴速度，返回字符串
QString PoseData::getSpeed_X() const {
    return QString::number(speedX_);
}

// 获取Y轴速度，返回字符串
QString PoseData::getSpeed_Y() const {
    return QString::number(speedY_);
}

// 获取线速度，返回字符串
QString PoseData::getSpeed() const {
    return QString::number(speed_);
}

// 获取农机车头角度，返回字符串
QString PoseData::getRotation() const {
    return QString::number(rotation_);
}

// 获取运动偏差，返回字符串
QString PoseData::getBias() const {
    return QString::number(bias_);
}
