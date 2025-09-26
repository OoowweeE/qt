#include "MachineStatus.hpp"

// 静态实例化单例
MachineStatus& MachineStatus::instance() {
    static MachineStatus instance;  // 局部静态变量，确保线程安全且只初始化一次
    return instance;
}

// 私有构造函数
MachineStatus::MachineStatus() {
    // 初始化所有农机状态数据
    tCutWidth_ = 0.0;
    tMainAntennaHeight_ = 0.0;
    tMainSubAntennaDistance_ = 0.0;
    tAntennaToAxisDistance_ = 0.0;
    tAntennaToFrontAxleDistance_ = 0.0;
    tWheelbase_ = 0.0;
    tFrontWheel_ = 0.0;
    tSpeed_ = 0.0;
    tMinTurningRadius_ = 0.0;
}

/*
 * 更新割幅
 */
void MachineStatus::updateCutWidth(double tCutWidth) {
    tCutWidth_ = tCutWidth; // 更新割幅
}

/*
 * 更新主天线高度
 */
void MachineStatus::updateMainAntennaHeight(double tMainAntennaHeight) {
    tMainAntennaHeight_ = tMainAntennaHeight; // 更新主天线高度
}

/*
 * 更新主副天线距离
 */
void MachineStatus::updateMainSubAntennaDistance(double tMainSubAntennaDistance) {
    tMainSubAntennaDistance_ = tMainSubAntennaDistance; // 更新主副天线距离
}

/*
 * 更新天线到中轴的距离
 */
void MachineStatus::updateAntennaToAxisDistance(double tAntennaToAxisDistance) {
    tAntennaToAxisDistance_ = tAntennaToAxisDistance; // 更新天线到中轴的距离
}

/*
 * 更新天线到前轴的距离
 */
void MachineStatus::updateAntennaToFrontAxleDistance(double tAntennaToFrontAxleDistance) {
    tAntennaToFrontAxleDistance_ = tAntennaToFrontAxleDistance; // 更新天线到前轴的距离
}

/*
 * 更新拖拉机的轴距
 */
void MachineStatus::updateWheelbase(double tWheelbase) {
    tWheelbase_ = tWheelbase; // 更新拖拉机的轴距
}

/*
 * 更新拖拉机前轮
 */
void MachineStatus::updateFrontWheel(double tFrontWheel) {
    tFrontWheel_ = tFrontWheel; // 更新拖拉机前轮
}

/*
 * 更新行驶速度
 */
void MachineStatus::updateSpeed(double tSpeed) {
    tSpeed_ = tSpeed; // 更新行驶速度
}

/*
 * 更新最小转弯半径-路径规划
 */
void MachineStatus::updateMinTurningRadius(double tMinTurningRadius) {
    tMinTurningRadius_ = tMinTurningRadius; // 更新最小转弯半径-路径规划
}

/*
 * 获取农机状态数据，返回格式化字符串
 */
QString MachineStatus::getMachineStatus() const {
    return QString("Cut Width: %1, Main Antenna Height: %2, Main-Sub Antenna Distance: %3, Antenna to Axis Distance: %4, Antenna to Front Axle Distance: %5, Wheelbase: %6, Front Wheel: %7, Speed: %8, Min Turning Radius: %9")
        .arg(tCutWidth_).arg(tMainAntennaHeight_).arg(tMainSubAntennaDistance_).arg(tAntennaToAxisDistance_)
        .arg(tAntennaToFrontAxleDistance_).arg(tWheelbase_).arg(tFrontWheel_).arg(tSpeed_).arg(tMinTurningRadius_);
}

// 获取割幅，返回字符串
QString MachineStatus::getCutWidth() const {
    return QString::number(tCutWidth_);
}

// 获取主天线高度，返回字符串
QString MachineStatus::getMainAntennaHeight() const {
    return QString::number(tMainAntennaHeight_);
}

// 获取主副天线距离，返回字符串
QString MachineStatus::getMainSubAntennaDistance() const {
    return QString::number(tMainSubAntennaDistance_);
}

// 获取天线到中轴的距离，返回字符串
QString MachineStatus::getAntennaToAxisDistance() const {
    return QString::number(tAntennaToAxisDistance_);
}

// 获取天线到前轴的距离，返回字符串
QString MachineStatus::getAntennaToFrontAxleDistance() const {
    return QString::number(tAntennaToFrontAxleDistance_);
}

// 获取拖拉机的轴距，返回字符串
QString MachineStatus::getWheelbase() const {
    return QString::number(tWheelbase_);
}

// 获取拖拉机前轮，返回字符串
QString MachineStatus::getFrontWheel() const {
    return QString::number(tFrontWheel_);
}

// 获取行驶速度，返回字符串
QString MachineStatus::getSpeed() const {
    return QString::number(tSpeed_);
}

// 获取最小转弯半径-路径规划，返回字符串
QString MachineStatus::getMinTurningRadius() const {
    return QString::number(tMinTurningRadius_);
}
