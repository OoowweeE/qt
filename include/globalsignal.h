#ifndef GLOBALSIGNAL_H
#define GLOBALSIGNAL_H

#include <QObject>
//#include <xlnt/xlnt.hpp>
#include "rapidcsv.h"                       //rapidcsv
#include <iostream>
#include <QList>
#include <QPointF>
#include <QProcess>
#include <QMessageBox>
#include <QApplication>
#include "sqchartview.h"
#include "sqchart.h"
#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QDir>

typedef struct {//定义结构体存储ros发送来的各种数据
    char modelVersion[20];    // 型号版本
    uint8_t deviceNumber;     // 设备号
    uint8_t statusCode;          // 状态码
    float latitude;                    // 纬度
    char latitudeUnit;              // 纬度单位 (N/S)
    float longitude;                  // 经度
    char longitudeUnit;            // 经度单位 (E/W)
    float altitude;                     // 高度
    char altitudeUnit;               // 高度单位 (m/ft)
    float speed;                        // 速度值
    char speddUnit;                 // 米速度单位（m/s）
    float yaw;
    float pitch;
    float guwuhanshuilv;        //答辩前两天我竟然还在改代码！！！
    float guwuliuliang;
    float shishimuchan;
    float dangqianchanliang;
} ANetData_t;

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

namespace globalValue {
    extern int globalVariable;     //测试全局变量是否可用
    extern QList<QPointF> readCsvPath;//载入历史csv路径
    extern bool flag_readPath;  //判断是否显示readPath这个item
    extern bool flag_launch_WayPointLoader;//是否开启路径读取（启动和关闭WayPointLoader）
    extern bool flag_launch_autoNavigation;//是否开启路径跟踪
    extern bool flag_launch_savePath;//路开始记录/路径停止记录
    extern ANetData_t ANetData;//创建ros存储对象
    extern canData_t canData;//创建lsd_can结构体

    extern const char* buttonStyle_UnChecked;
    extern const char* buttonStyle_Checked;
}



class GlobalSignal : public QObject
{
    Q_OBJECT
public:
    explicit GlobalSignal(QObject *parent = nullptr);
    static GlobalSignal* instance();
    //创建全局变量需要在cpp中文件定义

    QList<QPointF> ReadCSV_XY(const QString &filePath);

signals:
    void globalSignal_Fix(double x,double y);

    void globalSignal_update_FixGNSS(double,double,double);
    void globalSignal_update_finalPathXY(double,double);
    void globalSignal_update_basePathXY(double,double);
    void gloablSignal_update_vehicle_Pos(double,double);

    void globalSignal_update_velSpeed(double,double);

    void globalSignal_update_yaw(double);
    void globalSignal_update_roll(double);
    void globalSignal_update_pitch(double);
    void globalSignal_update_bds(int,int);

    void globalSignal_update_lsd_CAN(QString);

    void globalSignal_KeyPress();
private:


};

#endif // GLOBALSIGNAL_H
