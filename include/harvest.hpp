#ifndef HARVEST_HPP
#define HARVEST_HPP
#include "globalsignal.h"
class harvest{
public:
    // 总产量
    static float HarvestID1_totalYield(QString data ){
        // 分割字符串并提取前四个字节
        QStringList bytes = data.split(" ");
        // bytes[0]输出是0
        QString hexString = bytes[1] + bytes[2] + bytes[3] + bytes[4];  // 拼接前四个字节
        bool ok;
        int decimalValue = hexString.toInt(&ok, 16);
        if (ok) {
            // 将结果除以 1000
            double result = decimalValue / 1000.0;
            return  result;
        }else {
            qDebug() << "转换失败";
            return 0.0;
        }
    }

    // 谷物流量
    static float HarvestID3_grainFlowRate(QString data ){
        // 分割字符串并提取前四个字节
        QStringList bytes = data.split(" ");
        QString hexString = bytes[1] + bytes[2] + bytes[3] + bytes[4];  // 拼接前四个字节

        // 将拼接后的十六进制字符串转换为十进制整数
        bool ok;
        int decimalValue = hexString.toInt(&ok, 16);
        if (ok) {
            // 将结果除以 1000
            double result = decimalValue / 1000000.0;
            return  result;
        }else {
            qDebug() << "转换失败";
            return 0.0;
        }
    }

    // 谷物含水率
    static float HarvestID2_grainMoisture(QString data ){
        // 分割字符串并提取前四个字节
        QStringList bytes = data.split(" ");
        QString hexString = bytes[4] + bytes[3];  // 拼接前四个字节

        // 将拼接后的十六进制字符串转换为十进制整数
        bool ok;
        int decimalValue = hexString.toInt(&ok, 16);
        if (ok) {
            // 将结果除以 1000
            double result = decimalValue / 10.0;
            return  result;
    //        qDebug() << "十进制值:" << decimalValue;
    //        qDebug() << "除以 1000 的结果:" << result;
        }else {
            qDebug() << "转换失败";
            return 0.0;
        }
    }
};

#endif // HARVEST_H
