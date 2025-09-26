#ifndef YIELDDATA_H
#define YIELDDATA_H

#include <QString>
#include <QStringList>
#include <QDebug>
#include "rapidcsv.h"
#include <QDateTime>
class YieldData
{
public:
    // 获取单例实例
    static YieldData& instance();

    // 禁止复制构造和赋值
    YieldData(const YieldData&) = delete;
    YieldData& operator=(const YieldData&) = delete;
    typedef struct {//定义结构体存储ros发送来的各种数据
        uint32_t deviceID;            // 设备号
        QString canString;            // 16进制can原始数据
        float latitude;                    // 纬度
        char latitudeUnit;              // 纬度单位 (N/S)
        float longitude;                  // 经度
        char longitudeUnit;            // 经度单位 (E/W)
        float altitude;                     // 高度
        char altitudeUnit;               // 高度单位 (m/ft)
        float speedX;                      // 线速度x
        float speedY;                      // 线速度y
        float speed;                        // 速度值
        QString speedUnit;                 // 米速度单位（m/s）
    } canData_t;
    canData_t canData;

    // Setters
    void setCanGrainFlowRate(double rate); // 设置谷物流量
    void setCanGrainMoisture(double moisture); // 设置谷物含水率
    void setCanActualPerAcreYield(double yield); // 设置实时产量
    void setCanTotalYield(double yield); // 设置总产量
    void setYieldWorkTime(double time); // 设置作业时间

    // Getters
    double getCanGrainFlowRate() const; // 获取谷物流量
    double getCanGrainMoisture() const; // 获取谷物含水率
    double getCanActualPerAcreYield() const; // 获取实时产量
    double getCanTotalYield() const; // 获取总产量
    double getYieldWorkTime() const; // 获取作业时间

    void String2Struct_yield_CAN(QString canString);// 将QString类型到CAN数据解析至结构体类型的canData中
    static float HarvestID1_totalYield(QString data);
    static float HarvestID2_grainMoisture(QString data);
    static float HarvestID3_grainFlowRate(QString data);
    static void InitCSV(QString &filename);//初始化一个csv文件
    void CanSaveToCSV(QString &filename);//将can数据保存至csv文件
private:
    // 私有构造函数，确保只有一个实例
    YieldData();

    // 产量监测数据成员
    double canGrainFlowRate_;  // 谷物流量
    double canGrainMoisture_;  // 谷物含水率
    double canActualPerAcreYield_;  // 实时产量
    double canTotalYield_;  // 总产量
    double yieldWorkTime_;  // 作业时间
};

#endif // YIELDDATA_H
