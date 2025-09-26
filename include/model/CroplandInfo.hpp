#ifndef CROPLANDINFO_H
#define CROPLANDINFO_H

#include <QString>
#include <QDate>

class CroplandInfo
{
public:
    // 获取单例实例
    static CroplandInfo& instance();

    // 禁止复制构造和赋值
    CroplandInfo(const CroplandInfo&) = delete;
    CroplandInfo& operator=(const CroplandInfo&) = delete;

    // 农田及作物数据成员
    int id_;                     // 唯一标识符，作为主键
    QString driver_;     // 作业人
    QString fieldName_;  // 地块名
    double fieldArea_;  // 地块面积
    QString cropVariety_;  // 作物品种
    QString workType_;  // 作物品种
    QString weather_;  // 天气状况
    QString soilType_;  // 土壤类型
    QString workDate_;    // 作业日期
    QDate plantingDate_;  // 种植日期
    QDate harvestDate_; // 收获日期

    // Setters
    void setCropVariety(const QString& variety); // 设置作物品种
    void setFieldName(const QString& name); // 设置地块名
    void setFieldArea(double area); // 设置地块面积
    void setWeather(const QString& weather); // 设置天气状况
    void setSoilType(const QString& type); // 设置土壤类型
    void setPlantingDate(const QDate& date); // 设置种植日期
    void setHarvestDate(const QDate& date); // 设置收获日期

    // Getters
    QString getCropVariety() const; // 获取作物品种
    QString getFieldName() const; // 获取地块名
    double getFieldArea() const; // 获取地块面积
    QString getWeather() const; // 获取天气状况
    QString getSoilType() const; // 获取土壤类型
    QDate getPlantingDate() const; // 获取种植日期
    QDate getHarvestDate() const; // 获取收获日期

private:
    // 私有构造函数，确保只有一个实例
    CroplandInfo();


};

#endif // CROPLANDINFO_H
