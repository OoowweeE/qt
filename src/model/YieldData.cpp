#include "YieldData.hpp"

// 静态实例化单例
YieldData& YieldData::instance() {
    static YieldData instance;  // 局部静态变量，确保线程安全且只初始化一次
    return instance;
}

// 私有构造函数
YieldData::YieldData() :
    canData({0,"",0.0,'N',0.0,'E',0.0,'m',0.0,0.0,0.0,"m/s"}),//初始化can数据
    canGrainFlowRate_(0.0),
    canGrainMoisture_(0.0),
    canActualPerAcreYield_(0.0),
    canTotalYield_(0.0),
    yieldWorkTime_(0.0){
    // 初始化产量监测数据为零
}

void YieldData::String2Struct_yield_CAN(QString canString)
{
    canData = {0, "", 0.0, 'N', 0.0, 'E', 0.0, 'm', 0.0, 0.0, 0.0, "m/s"};//重置结构体变量（清空）
    QStringList canDataList = canString.split(",");//用逗号分割
    canData.deviceID = canDataList[0].trimmed().toUInt();
    canData.canString = canDataList[1].trimmed();
    canData.latitude = canDataList[2].trimmed().toFloat();
    canData.latitudeUnit = 'N';
    canData.longitude = canDataList[3].trimmed().toFloat();
    canData.longitudeUnit = 'E';
    canData.speedX = canDataList[4].trimmed().toFloat();
    canData.speedY = canDataList[5].trimmed().toFloat();
    canData.speed = canDataList[6].trimmed().toFloat();
    canData.speedUnit = "m/s";
    qDebug() << "Device ID:" << canData.deviceID;
    qDebug() << "CAN String:" << canData.canString;
    qDebug() << "Latitude:" << canData.latitude << canData.latitudeUnit;
    qDebug() << "Longitude:" << canData.longitude << canData.longitudeUnit;
    qDebug() << "Altitude:" << canData.altitude << canData.altitudeUnit;
    qDebug() << "Speed X:" << canData.speedX;
    qDebug() << "Speed Y:" << canData.speedY;
    qDebug() << "Speed:" << canData.speed << canData.speedUnit;
}

// 解析总产量
float YieldData::HarvestID1_totalYield(QString data){
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

// 解析谷物含水率
float YieldData::HarvestID2_grainMoisture(QString data ){
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

// 谷物流量
float YieldData::HarvestID3_grainFlowRate(QString data ){
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

//初始化一个csv文件
void YieldData::InitCSV(QString &filename)
{
    // 创建一个 rapidcsv::Document
    rapidcsv::Document doc("", rapidcsv::LabelParams(0, -1));  // 没有列名，有行名(第一行)

    // 设置列名
    doc.SetColumnName(0, "Device ID");
    doc.SetColumnName(1, "CAN String");
    doc.SetColumnName(2, "Latitude");
    doc.SetColumnName(3, "Longitude");
    doc.SetColumnName(4, "Speed X");
    doc.SetColumnName(5, "Speed Y");
    doc.SetColumnName(6, "Speed");
    doc.SetColumnName(7, "Time");

    // 保存初始化后的 CSV 文件
    std::string savePath = filename.toStdString();
    doc.Save(savePath);
    qDebug() << "CSV 文件初始化完成并已保存为:  " << filename;
}

void YieldData::CanSaveToCSV(QString &filename)
{
    std::string savePath = filename.toStdString();
    // 创建一个 rapidcsv::Document
    rapidcsv::Document doc(savePath, rapidcsv::LabelParams(0, -1));  // 没有列名，有行名(第一行)
    // 找到当前文件的行数，以便在最后一行添加新数据
    long rowCount = doc.GetRowCount();
    // 准备数据
    std::vector<std::string> rowData;
    //rowData.push_back(std::to_string(canData_tmp.deviceID));                 // Device ID（10进制保存）
    rowData.push_back(QString::number(canData.deviceID, 16).toUpper().toStdString());                 // Device ID (16进制保存)
    rowData.push_back(canData.canString.toStdString());                                       // CAN String
    rowData.push_back(std::to_string(canData.latitude));                  // Latitude
    rowData.push_back(std::to_string(canData.longitude));               // Longitude
    rowData.push_back(std::to_string(canData.speedX));                   // Speed X
    rowData.push_back(std::to_string(canData.speedY));                   // Speed Y
    rowData.push_back(std::to_string(canData.speed));                     // Speed
    //获取当前时间并格式化为 "yyyy-MM-dd HH:mm:ss" 格式
    QString dateTimeString = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    rowData.push_back(dateTimeString.toStdString());                     // Speed
    // 插入数据
    doc.SetRow<std::string>(rowCount, rowData);
    // 保存 CSV 文件
    doc.Save(savePath);
}
/*
 * 设置谷物流量
 */
void YieldData::setCanGrainFlowRate(double rate) {
    canGrainFlowRate_ = rate;  // 更新谷物流量
}

/*
 * 设置谷物含水率
 */
void YieldData::setCanGrainMoisture(double moisture) {
    canGrainMoisture_ = moisture;  // 更新谷物含水率
}

/*
 * 设置实时产量
 */
void YieldData::setCanActualPerAcreYield(double yield) {
    canActualPerAcreYield_ = yield;  // 更新实时产量
}

/*
 * 设置总产量
 */
void YieldData::setCanTotalYield(double yield) {
    canTotalYield_ = yield;  // 更新总产量
}

/*
 * 设置作业时间
 */
void YieldData::setYieldWorkTime(double time) {
    yieldWorkTime_ = time;  // 更新作业时间
}

/*
 * 获取谷物流量
 */
double YieldData::getCanGrainFlowRate() const {
    return canGrainFlowRate_;  // 返回谷物流量
}

/*
 * 获取谷物含水率
 */
double YieldData::getCanGrainMoisture() const {
    return canGrainMoisture_;  // 返回谷物含水率
}

/*
 * 获取实时产量
 */
double YieldData::getCanActualPerAcreYield() const {
    return canActualPerAcreYield_;  // 返回实时产量
}

/*
 * 获取总产量
 */
double YieldData::getCanTotalYield() const {
    return canTotalYield_;  // 返回总产量
}

/*
 * 获取作业时间
 */
double YieldData::getYieldWorkTime() const {
    return yieldWorkTime_;  // 返回作业时间
}
