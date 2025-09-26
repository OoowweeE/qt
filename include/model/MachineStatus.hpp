#ifndef MACHINESTATUS_H
#define MACHINESTATUS_H

#include <QString>

class MachineStatus
{
public:
    // 获取单例实例
    static MachineStatus& instance();

    // 禁止复制构造和赋值
    MachineStatus(const MachineStatus&) = delete;
    MachineStatus& operator=(const MachineStatus&) = delete;

    // 存储农机状态数据
    QString tKind_;
    QString tId_;
    double tCutWidth_;
    double tMainAntennaHeight_;
    double tMainSubAntennaDistance_;
    double tAntennaToAxisDistance_;
    double tAntennaToFrontAxleDistance_;
    double tWheelbase_;
    double tFrontWheel_;
    double tSpeed_;
    double tMinTurningRadius_;

    // update——存储/更新农机状态数据
    void updateCutWidth(double tCutWidth);
    void updateMainAntennaHeight(double tMainAntennaHeight);
    void updateMainSubAntennaDistance(double tMainSubAntennaDistance);
    void updateAntennaToAxisDistance(double tAntennaToAxisDistance);
    void updateAntennaToFrontAxleDistance(double tAntennaToFrontAxleDistance);
    void updateWheelbase(double tWheelbase);
    void updateFrontWheel(double tFrontWheel);
    void updateSpeed(double tSpeed);
    void updateMinTurningRadius(double tMinTurningRadius);

    // get——获取数据
    QString getMachineStatus() const;
    QString getCutWidth() const;
    QString getMainAntennaHeight() const;
    QString getMainSubAntennaDistance() const;
    QString getAntennaToAxisDistance() const;
    QString getAntennaToFrontAxleDistance() const;
    QString getWheelbase() const;
    QString getFrontWheel() const;
    QString getSpeed() const;
    QString getMinTurningRadius() const;

private:
    // 私有构造函数，确保只有一个实例
    MachineStatus();


};

#endif // MACHINESTATUS_H
