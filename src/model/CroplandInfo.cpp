#include "CroplandInfo.hpp"

// 静态实例化单例
CroplandInfo& CroplandInfo::instance() {
    static CroplandInfo instance;  // 局部静态变量，确保线程安全且只初始化一次
    return instance;
}

// 私有构造函数
CroplandInfo::CroplandInfo() :
    fieldArea_(0.0) {
    // 初始化默认值
    cropVariety_ = "";
    fieldName_ = "";
    weather_ = "";
    soilType_ = "";
    plantingDate_ = QDate::currentDate();  // 默认为当前日期
    harvestDate_ = QDate::currentDate();  // 默认为当前日期
}

/*
 * 设置作物品种
 */
void CroplandInfo::setCropVariety(const QString& variety) {
    cropVariety_ = variety;  // 更新作物品种
}

/*
 * 设置地块名
 */
void CroplandInfo::setFieldName(const QString& name) {
    fieldName_ = name;  // 更新地块名
}

/*
 * 设置地块面积
 */
void CroplandInfo::setFieldArea(double area) {
    fieldArea_ = area;  // 更新地块面积
}

/*
 * 设置天气状况
 */
void CroplandInfo::setWeather(const QString& weather) {
    weather_ = weather;  // 更新天气状况
}

/*
 * 设置土壤类型
 */
void CroplandInfo::setSoilType(const QString& type) {
    soilType_ = type;  // 更新土壤类型
}

/*
 * 设置种植日期
 */
void CroplandInfo::setPlantingDate(const QDate& date) {
    plantingDate_ = date;  // 更新种植日期
}

/*
 * 设置收获日期
 */
void CroplandInfo::setHarvestDate(const QDate& date) {
    harvestDate_ = date;  // 更新收获日期
}

/*
 * 获取作物品种
 */
QString CroplandInfo::getCropVariety() const {
    return cropVariety_;  // 返回作物品种
}

/*
 * 获取地块名
 */
QString CroplandInfo::getFieldName() const {
    return fieldName_;  // 返回地块名
}

/*
 * 获取地块面积
 */
double CroplandInfo::getFieldArea() const {
    return fieldArea_;  // 返回地块面积
}

/*
 * 获取天气状况
 */
QString CroplandInfo::getWeather() const {
    return weather_;  // 返回天气状况
}

/*
 * 获取土壤类型
 */
QString CroplandInfo::getSoilType() const {
    return soilType_;  // 返回土壤类型
}

/*
 * 获取种植日期
 */
QDate CroplandInfo::getPlantingDate() const {
    return plantingDate_;  // 返回种植日期
}

/*
 * 获取收获日期
 */
QDate CroplandInfo::getHarvestDate() const {
    return harvestDate_;  // 返回收获日期
}