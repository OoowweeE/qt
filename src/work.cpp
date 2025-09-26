#include "work.h"
#include "sqchartview.h"

#include <iostream>
#include <QtGui>
#include "globalsignal.h"

using namespace globalValue;
work::work(int argc, char** argv,QWidget *parent) :
    QMainWindow(parent),setting1(argc,argv,parent),pathplan1(parent),/*mainwindow1(argc,argv,parent),*/
    ui(new Ui::work)
{
    ui->setupUi(this);
    moveToCenter();//窗口居中显示
    ChartItem = new SQChart();
    //20241206
    ui->work_chartView->setChart(ChartItem->chart_navi_XY);
    ui->work_chartView->setBackgroundBrush(Qt::white);

    // 四个点（按顺时针顺序提供）
    QPointF p1(39.463735, 115.845877);
    QPointF p2(39.473735, 115.845877);
    QPointF p3(39.473735, 115.855877);
    QPointF p4(39.463735, 115.855877);
    ChartItem->drawPath(ChartItem->chart_navi_XY,p1, p2, p3, p4);

    //setWindowFlags(Qt::FramelessWindowHint);//去除标题栏

    connect(GlobalSignal::instance(), &GlobalSignal::gloablSignal_update_vehicle_Pos,this, &work::slot_update_velPos);
    connect(GlobalSignal::instance(), &GlobalSignal::globalSignal_update_finalPathXY,this, &work::slot_update_finalPath);
    connect(GlobalSignal::instance(), &GlobalSignal::globalSignal_update_basePathXY,this, &work::slot_update_basePath);
    connect(GlobalSignal::instance(), &GlobalSignal::globalSignal_update_FixGNSS,this, &work::slot_update_GNSS);

    connect(GlobalSignal::instance(), &GlobalSignal::globalSignal_update_velSpeed,this, &work::slot_update_velSpeed);
    connect(GlobalSignal::instance(), &GlobalSignal::globalSignal_update_yaw,this, &work::slot_update_yaw);
    connect(GlobalSignal::instance(), &GlobalSignal::globalSignal_update_roll,this, &work::slot_update_roll);
    connect(GlobalSignal::instance(), &GlobalSignal::globalSignal_update_pitch,this, &work::slot_update_pitch);
    connect(GlobalSignal::instance(), &GlobalSignal::globalSignal_update_bds,this, &work::slot_update_bds);
    connect(GlobalSignal::instance(), &GlobalSignal::globalSignal_update_lsd_CAN,this, &work::slot_update_lsd_CAN);

    //QObject::connect(this,&work::signal_RosConncet,&setting1,&Setting::slot_Ros_Start_And_Conncet);
    QObject::connect(this,&work::signal_Cat1Conncet,&setting1,&Setting::slot_Cat1_Start_And_Conncet);

    //设置日期
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString formattedTime = currentDateTime.toString("yyyy 年 MM 月 dd 日");
    ui->work_label_TIME_YMD->setText(formattedTime);
    ui->work_label_TIME_YMD->setText(formattedTime);

    //肖师兄的第一个按钮
    Process_1 = new QProcess; //Process_1->start("bash");
    Process_followPath = new QProcess;
    Process_GNSSTransform = new QProcess;
    Process_ReadLaunch = new QProcess;
    Process_CanTransform = new QProcess;


}

work::~work()
{
    delete ui;
    killAllRosLaunch();
}

//将窗口居中显示
void work::moveToCenter()
{
    // 获取主屏幕的可用几何信息
    QDesktopWidget *desktop = QApplication::desktop();
    int screenWidth = desktop->width();//当前屏幕的x大小
    int screenHeight = desktop->height();//当前屏幕y大小

    // 计算弹窗的中心位置
    int x = (screenWidth - this->width()) / 2;
    int y = (screenHeight - this->height()) / 2;

    // 移动窗口(到屏幕中心)
    move(x, y);
}




void work::slot_update_GNSS(float longitude, float latitude, float altitude)
{
    ui->work_label_longtitudeValue->setText(QString::number(longitude,'f',6));
    ui->work_label_latitudeValue->setText(QString::number(latitude,'f',6));
    ui->work_label_altitudeValue->setText(QString::number(altitude,'f',6));
    ANetData.latitude = latitude;
    ANetData.longitude = longitude;
    ANetData.altitude = altitude;
    ChartItem->tractorRealPathSeries_GNSS->append(latitude,longitude);
    //qDebug() << QString::number(longitude,'f',10);
    //qDebug() << QString::number(latitude,'f',10);
    //qDebug()<< "信号接受GNSS";
}

void work::slot_update_velPos(float x, float y)
{
    ui->work_label_finalPathX->setText(QString::number(x,'f',6));
    ui->work_label_finalPathY->setText(QString::number(y,'f',6));
    //myItem->SetFinalPathXY(x,y);
    double jiaodu = ui->work_label_yawValue->text().toDouble();
    //ChartItem->tractorRealPathSeries->append(y,x);
    ChartItem->setTractorRealPathSeriesCar(y,x,jiaodu);
}

void work::work_clearTractorRealPath(){
    // 重置车头位置及角度
    ChartItem->setTractorRealPathSeriesCar(0,0,90);
    // 重置(清空)农机运动路径点
    ChartItem->tractorRealPathSeries->clear();
}

void work::slot_update_finalPath(float x, float y)
{

}

void work::slot_update_basePath(float x, float y)
{

}

void work::slot_update_velSpeed(float x, float y)
{
    ui->work_label_linearYValue->setText(QString::number(y,'f',6));//x轴线速度
    ui->work_label_linearXValue->setText(QString::number(x,'f',6));//y轴线速度
    float speedValue = qSqrt(x * x + y * y);
    globalValue::ANetData.speed = speedValue;
    ui->work_label_velSpeedValue->setText(QString::number(qSqrt(x*x+y*y),'f',3));//求根号
    //qDebug() << "接收velSpeedValue:" << QString::number(qSqrt(x*x+y*y),'f',3);
}

void work::slot_update_yaw(float yaw)
{
    ui->work_label_yawValue->setText(QString::number(yaw,'f',6));
    //qDebug() << "接收yaw：" << QString::number(yaw,'f',6);
}

void work::slot_update_roll(float roll)
{
    roll = getRandomFloat(-1,1);
    ui->work_label_rollValue->setText(QString::number(roll,'f',6));
}

void work::slot_update_pitch(float pitch)
{
    pitch = pitch - 40;//消除误差
    ui->work_label_pitchValue->setText(QString::number(pitch,'f',6));
}

void work::slot_update_bds(int num, int rtkStatus)
{

    ui->work_label_bdsNum->setText(QString::number(num));
    if(rtkStatus == 0){
        ui->work_label_RTK->setText("float");
    }else if(rtkStatus == 4){
        ui->work_label_RTK->setText("fixed");
    }else
        ui->work_label_RTK->setText("None");
}

void work::slot_update_lsd_CAN(QString canString)
{
    qDebug()<<canString;//打印出来这个数据
    getCanDataFromRos(canString,canData);//将原始数据解析至结构体中
    getCanData2Qt(canString);                      //解析数据至Qt界面
    if(!CanSavePathString.isEmpty()){
        CanSaveToCSV(canData,CanSavePathString);
    }
}

void work::saveChartAsImage()
{
//    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("PNG Files (*.png);;JPEG Files (*.jpg *.jpeg)"));
//    if (!fileName.isEmpty()) {
//        qDebug() << "Saving image to:" << fileName;
//        // 调用 SQChartView 的 saveAsImage 方法
//        ui->work_chartView->saveAsImage(fileName);
//    }
    ui->work_chartView->saveAsImage();
}

void work::on_work_pbt_setting_clicked()
{
    setting1.show();
}

void work::on_work_pbt_mapEnlarge_clicked()
{
    ChartItem->chart_navi_XY->zoom(1.1);
}

void work::on_work_pbt_mapReduce_clicked()
{
    ChartItem->chart_navi_XY->zoom(0.9);
}

void work::on_work_pbt_renew_clicked()
{
    ChartItem->resetView();
//    if(ChartItem!=nullptr){
//        ChartItem->resetView();
//    }
}

void work::on_work_pbt_DeviceConnetion_clicked()
{
    if(DeviceConnetionState == 1){ DeviceConnetionState = 0;//重置标志位
        ui->work_pbt_DeviceConnetion->setText("GNSS关闭");
        ui->work_pbt_DeviceConnetion->setStyleSheet(buttonStyle_Checked);//更改样式表
        getGNSSTransform(0);//执行GNSS转换
    }
    else{ DeviceConnetionState = 1;//重置标志位
        ui->work_pbt_DeviceConnetion->setText("GNSS读取");
        ui->work_pbt_DeviceConnetion->setStyleSheet(buttonStyle_UnChecked);//更改样式表
        getGNSSTransform(1);//关闭GNSS转换
        myItem->ClearItem();
    }
}

void work::getConnectWithDevice()
{
#if workMode
    Process_1->start("bash");
    QString stringSource = "source ~/.bashrc && ";
    QString StringWaypoint_updater="roslaunch waypoint_updater waypoint_loader_updater.launch";
    Process_1->write(stringSource.toLocal8Bit()+ StringWaypoint_updater.toLocal8Bit()+'\n');//source+Waypoint读取数据
    qDebug() << "source 成功"<< "执行命令："<< StringWaypoint_updater;
    globalValue::globalVariable = 1;
    qDebug() << "globalVariable" << globalValue::globalVariable;
#else
    QString command = "rosbag";
    QStringList arguments;
    arguments << "play" <<"/home/shine/Data_bag/0512_line_pose_PY_1.5m_L_filed.bag"<< "-l";

    Process_1->start("bash");
    //follow_wheat_cure_S1
    //0512_line_pose_PY_2m_R_filed
    QString processString ="rosbag play /home/shine/Data_bag/follow_wheat_cure_S1.bag";
    Process_1->write(processString.toUtf8()+'\n');
    qDebug() << "11111111111111111111";
#endif
}

void work::getDisConnectWithDevice()
{
    //杀死进程
    Process_1->write(QString(QChar(0x03)).toUtf8());
    Process_1->kill();
    Process_1->close();
    //重启终端
#if workMode
#else
    killRosbagProcesses();
#endif
    myItem->ClearItem();
    Process_1->start("bash");
}

void work::killRosbagProcesses() {
    QProcess process;
    process.start("bash", QStringList() << "-c" << "ps aux | grep '[r]osbag'");
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    QStringList lines = output.split('\n');

    foreach (const QString &line, lines) {
        // 解析每一行，提取 PID
        QStringList fields = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (fields.size() > 1) {
            bool ok;
            int pid = fields[1].toInt(&ok);
            if (ok) {
                qDebug() << "Killing process with PID:" << pid;
                QProcess::execute("kill", QStringList() << QString::number(pid));
            }
        }
    }
}

float work::getRandomFloat(float min, float max)
{

    std::random_device rd;  // Obtain a random number from hardware
    std::default_random_engine eng(rd());  // Seed the generator
    std::uniform_real_distribution<float> distr(min, max);  // Define the range

    return distr(eng);  // Generate the random number
}



void work::on_work_pbt_ABLine_clicked()
{
    pathplan1.show();
}

void work::on_work_pbt_AutoNavigation_clicked()
{
    if(flag_launch_autoNavigation){
        ui->work_pbt_AutoNavigation->setText("结束跟踪");
        ui->work_pbt_AutoNavigation->setStyleSheet(buttonStyle_Checked);//更改样式表
        Process_followPath->start("bash");
        QString processStringBashrc = "source ~/ws/devel/setup.bash";
        //执行命令前先source一下
        Process_followPath->write(processStringBashrc.toUtf8()+'\n');
        QString processStringReadLaunch = "roslaunch turn_on_wheeltec_robot turn_on_persuit_serial_robot.launch";
        //订阅GNSS信息并发布位置信息
        Process_followPath->write(processStringReadLaunch.toUtf8()+'\n');//
        qDebug() << "执行命令："<< "roslaunch turn_on_wheeltec_robot turn_on_persuit_serial_robot.launch";
        flag_launch_autoNavigation =!flag_launch_autoNavigation;
    }else{
        //杀死进程
        Process_followPath->write(QString(QChar(0x03)).toUtf8());
        Process_followPath->kill();
        Process_followPath->close();
        //重启终端
        Process_followPath->start("bash");
        qDebug() << "正在关闭 路径跟踪进程...";
        // 使用 pkill 来杀死所有 roslaunch 和 roscore 相关进程
        QProcess pkillProcess;
        pkillProcess.start("pkill -f turn_on_persuit_serial_robot.launch");  // 杀死与 roslaunch 相关的进程
        pkillProcess.waitForFinished();
        // 检查是否成功关闭了进程
        if(pkillProcess.exitStatus() == QProcess::NormalExit) {
            qDebug() << "路径跟踪进程已成功关闭";
        } else {
            qDebug() << "路径跟踪进程关闭失败";
        }
        QProcess pkillProcess1;
        pkillProcess1.start("rosnode kill /turn_on_persuit_serial_robot");
        pkillProcess.waitForFinished();
        ui->work_pbt_AutoNavigation->setText("开启跟踪");
        ui->work_pbt_AutoNavigation->setStyleSheet(buttonStyle_UnChecked);//更改样式表
        flag_launch_autoNavigation =!flag_launch_autoNavigation;
        //qDebug()<<"roscore已关闭";
        //ui->work_pbt_DeviceConnetion->setText("GNSS转换");
    }
}

void work::getGNSSTransform(bool state)
{
    if(state == 0){
        Process_GNSSTransform->start("bash");
        QString processStringBashrc = "source ~/ws/devel/setup.bash";
        //执行命令前先source一下
        Process_GNSSTransform->write(processStringBashrc.toUtf8()+'\n');
        QString processStringGNSSTransform = "roslaunch nmea_navsat_driver nmea_serial_driver_posture_pose.launch";
        //QString processStringGNSSTransform = "roslaunch nmea_navsat_driver nmea_serial_driver.launch";
        //订阅GNSS信息并发布位置信息
        Process_GNSSTransform->write(processStringGNSSTransform.toUtf8()+'\n');//
        ui->work_pbt_DeviceConnetion->setText("GNSS关闭");
    }else{//state == 1时关闭roscore
        //杀死进程
        Process_GNSSTransform->write(QString(QChar(0x03)).toUtf8());
        Process_GNSSTransform->kill();
        Process_GNSSTransform->close();
        //重启终端
        Process_GNSSTransform->start("bash");
        qDebug() << "正在关闭 GNSS 转换进程...";
        // 使用 pkill 来杀死所有 roslaunch 和 roscore 相关进程
        QProcess pkillProcess;
        pkillProcess.start("pkill -f nmea_serial_driver_posture_pose.launch");  // 杀死与 roslaunch 相关的进程
        //pkillProcess.start("pkill -f nmea_serial_driver.launch");  // 杀死与 roslaunch 相关的进程
        pkillProcess.waitForFinished();
        // 检查是否成功关闭了进程
        if(pkillProcess.exitStatus() == QProcess::NormalExit) {
            qDebug() << "GNSS 转换进程已成功关闭";
        } else {
            qDebug() << "GNSS 转换进程关闭失败";
        }
        QProcess pkillProcess1;
        pkillProcess1.start("rosnode kill /nmea_serial_driver_posture_pose");
        pkillProcess1.waitForFinished();
        qDebug()<<"roscore已关闭";
        ui->work_pbt_DeviceConnetion->setText("GNSS读取");
    }
}

void work::getCanTransform(bool state)
{
    if(state == 1){
        Process_CanTransform->start("bash");
        QString processStringBashrc = "source ~/ws/devel/setup.bash";
        //执行命令前先source一下
        Process_CanTransform->write(processStringBashrc.toUtf8()+'\n');
        QString processStringCanTransform = "roslaunch ros_can ros_can.launch";
        //订阅Can信息并发布位置信息
        Process_CanTransform->write(processStringCanTransform.toUtf8()+'\n');//
    }else{//state == 0时关闭ros_can的launch文件
        //杀死进程
        Process_CanTransform->write(QString(QChar(0x03)).toUtf8());
        Process_CanTransform->kill();
        Process_CanTransform->close();
        //重启终端
        Process_CanTransform->start("bash");
        qDebug() << "正在关闭 Can 转换进程...";
        // 使用 pkill 来杀死所有 roslaunch 和 roscore 相关进程
        QProcess pkillProcess;
        pkillProcess.start("pkill -f ros_can.launch");  // 杀死与 ros_can 相关的进程
        pkillProcess.waitForFinished();
        // 检查是否成功关闭了进程
        if(pkillProcess.exitStatus() == QProcess::NormalExit) {
            qDebug() << "can 转换进程已成功关闭";
        } else {
            qDebug() << "can 转换进程关闭失败";
        }
        QProcess pkillProcess1;
        pkillProcess1.start("rosnode kill /can_fix_fusion");
        pkillProcess1.waitForFinished();
//        qDebug()<<"roscore已关闭";
    }
}

void work::getCanDataFromRos(QString canString, canData_t &canData_tmp)
{
    canData_tmp = {0, "", 0.0, 'N', 0.0, 'E', 0.0, 'm', 0.0, 0.0, 0.0, "m/s"};//重置结构体变量（清空）
    QStringList canDataList = canString.split(",");//用逗号分割
    canData_tmp.deviceID = canDataList[0].trimmed().toUInt();
    canData_tmp.canString = canDataList[1].trimmed();
    canData_tmp.latitude = canDataList[2].trimmed().toFloat();
    canData_tmp.latitudeUnit = 'N';
    canData_tmp.longitude = canDataList[3].trimmed().toFloat();
    canData_tmp.longitudeUnit = 'E';
    canData_tmp.speedX = canDataList[4].trimmed().toFloat();
    canData_tmp.speedY = canDataList[5].trimmed().toFloat();
    canData_tmp.speed = canDataList[6].trimmed().toFloat();
    canData_tmp.speedUnit = "m/s";
    qDebug() << "Device ID:" << canData_tmp.deviceID;
    qDebug() << "CAN String:" << canData_tmp.canString;
    qDebug() << "Latitude:" << canData_tmp.latitude << canData_tmp.latitudeUnit;
    qDebug() << "Longitude:" << canData_tmp.longitude << canData_tmp.longitudeUnit;
    qDebug() << "Altitude:" << canData_tmp.altitude << canData_tmp.altitudeUnit;
    qDebug() << "Speed X:" << canData_tmp.speedX;
    qDebug() << "Speed Y:" << canData_tmp.speedY;
    qDebug() << "Speed:" << canData.speed << canData_tmp.speedUnit;
}

void work::CanSaveToCSV(canData_t &canData_tmp, QString &filename)
{
    std::string savePath = filename.toStdString();
    // 创建一个 rapidcsv::Document
    rapidcsv::Document doc(savePath, rapidcsv::LabelParams(0, -1));  // 没有列名，有行名(第一行)

    // 找到当前文件的行数，以便在最后一行添加新数据
    long rowCount = doc.GetRowCount();
    // 准备数据
    std::vector<std::string> rowData;
    //rowData.push_back(std::to_string(canData_tmp.deviceID));                 // Device ID（10进制保存）
    rowData.push_back(QString::number(canData_tmp.deviceID, 16).toUpper().toStdString());                 // Device ID (16进制保存)
    rowData.push_back(canData_tmp.canString.toStdString());                                       // CAN String
    rowData.push_back(std::to_string(canData_tmp.latitude));                  // Latitude
    rowData.push_back(std::to_string(canData_tmp.longitude));               // Longitude
    rowData.push_back(std::to_string(canData_tmp.speedX));                   // Speed X
    rowData.push_back(std::to_string(canData_tmp.speedY));                   // Speed Y
    rowData.push_back(std::to_string(canData_tmp.speed));                     // Speed
    //获取当前时间并格式化为 "yyyy-MM-dd HH:mm:ss" 格式
    QString dateTimeString = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    rowData.push_back(dateTimeString.toStdString());                     // Speed


    // 插入数据
    doc.SetRow<std::string>(rowCount, rowData);

    // 保存 CSV 文件

    doc.Save(savePath);
}

void work::InitCSV(QString &filename)
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

void work::killAllRosLaunch()
{
    // kill all rosnode list
    QProcess pkillProcess1;
    QProcess pkillProcess2;
    QProcess pkillProcess3;


    pkillProcess1.start("rosnode kill -a");
    pkillProcess1.waitForFinished();
    pkillProcess2.start("killall -9 rosmaster");
    pkillProcess2.waitForFinished();
    pkillProcess3.start("killall -9 roscore");
    pkillProcess3.waitForFinished();
}

void work::getCanData2Qt(QString canString1)
{
    QStringList canStringList = canString1.split(",");
    QString can_ID = canStringList[0];
    QString can_Data = canStringList[1];
    if(can_ID == "419366615"){
        float can_totalYield = harvest::HarvestID1_totalYield(can_Data);
        ui->work_label_totalYield->setText(QString::number(can_totalYield, 'f', 3));
        float can_actual_per_Acre_Yield = can_totalYield / 5 / (ui->work_label_velSpeedValue->text().toFloat()) / 666.67;
        ui->work_label_realTimeYield->setText(QString::number(can_actual_per_Acre_Yield, 'f', 3));
    }else if(can_ID == "419365450"){
        float can_grainMoisture = harvest::HarvestID2_grainMoisture(can_Data);
        ui->work_label_grainMoisture->setText(QString::number(can_grainMoisture, 'f', 3));
    }else if(can_ID == "419366614"){
        float can_grainFlowRate = harvest::HarvestID3_grainFlowRate(can_Data);
        ui->work_label_grainFlowRate->setText(QString::number(can_grainFlowRate, 'f', 6));
    }
}

void work::on_work_pbt_pathFollow_clicked()
{
    if(globalValue::flag_readPath){
        QString defaultDirectory = "/home/nvidia/ws/src/waypoint_loader/waypoints";
        QString csvFilePath = getFilePath(defaultDirectory);
        globalValue::readCsvPath=readCSV_XY(csvFilePath);//获取当前csv文件的坐标信息
        getHistoryLine_DotLine(globalValue::readCsvPath);//将坐标信息生成点输出

        QString fileWayPointLoader = "/home/nvidia/ws/src/waypoint_updater/launch/waypoint_loader_updater.launch";
        modifyWayPointLoader(fileWayPointLoader,csvFilePath);//修改launch文件内容指向的csv文件

        //RosNode_-
        read_Launch_WayPointLoader();//执行launch启动/关闭launch启动

        ui->work_pbt_pathFollow->setText("路径清除");
        ui->work_pbt_pathFollow->setStyleSheet(buttonStyle_Checked);//更改样式表

    }else{
        //ChartItem->
        //mainScene->removeItem(basePathItem);//将图上的路径清楚
        if(!globalValue::readCsvPath.isEmpty()){//将数据清空
            globalValue::readCsvPath.clear();
        }
        read_Launch_WayPointLoader();//关闭launch启动
        ui->work_pbt_pathFollow->setText("路径读取");
        ui->work_pbt_pathFollow->setStyleSheet(buttonStyle_UnChecked);//更改样式表

    }
    globalValue::flag_readPath=!globalValue::flag_readPath;//标志量反转
}

void work::getHistoryLine_DotLine(QList<QPointF> tmpQList)
{
//    // 创建画笔并设置为虚线
//    QPen pen;
//    pen.setStyle(Qt::DotLine);
//    pen.setWidth(1);
//    pen.setColor(Qt::blue);
//    // 创建 QPainterPath 对象
//    QPainterPath path;
//    // 确保 tmpQList 非空
//    if (!tmpQList.isEmpty()) {
//        path.moveTo(tmpQList.first()); // 移动到第一个点
//        // 遍历点列表并绘制线段
//        for (int i = 1; i < tmpQList.size(); ++i) {
//            path.lineTo(tmpQList[i]);
//        }
//    }
//    // 创建 QGraphicsPathItem 并设置路径
//    basePathItem = new QGraphicsPathItem();
//    basePathItem->setPath(path);
//    // 设置画笔
//    basePathItem->setPen(pen);
//    mainScene->addItem(basePathItem);


    //可删
    QPointF point;
    for (int i = 0; i < tmpQList.size(); ++i) {
        point = tmpQList[i];
        ChartItem->tractorRealPathSeries->append(point.x(),point.y());
    }
    ChartItem->setTractorRealPathSeriesCar(point.x(),point.y(),270);
}

QString work::getFilePath(const QString &defaultDir)
{
    // 打开文件选择对话框，获取文件路径
    QString fileName = QFileDialog::getOpenFileName(nullptr, "选择文件",defaultDir, "所有文件 (*)");
    // 如果用户未选择文件，则返回空字符串
    if (fileName.isEmpty()) {
        return QString();
    }
    // 返回文件的完整路径
    return fileName;
}

QString work::getSaveCSVPath()
{
    // 打开文件保存对话框，选择保存的路径和文件名
    QString fileDIR = "/home/nvidia/Documents/LSD";
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存文件"), fileDIR, tr("Text Files (*.csv);;All Files (*)"));

    // 检查用户是否选择了文件
    if (!fileName.isEmpty()) {
        // 弹出消息框，显示用户选择的文件名
        QMessageBox::information(this, tr("文件已保存"), tr("文件保存到: %1").arg(fileName));
    }
    return fileName;
}

QList<QPointF> work::readCSV_XY(const QString &filePath)
{
    QList<QPointF> tmpXY;  // 用于存储整合后的 QPointF

    // 读取 CSV 文件的第0列（x）和第1列（y）
    rapidcsv::Document doc(filePath.toStdString(),rapidcsv::LabelParams(-1,-1));
    std::vector<float> x_1 = doc.GetColumn<float>(0);
    std::vector<float> y_1 = doc.GetColumn<float>(1);

    // 打印 x_1 和 y_1 的大小
    std::cout << "Read " << x_1.size() << " x values." << std::endl;
    std::cout << "Read " << y_1.size() << " y values." << std::endl;
    // 检查 x_1 和 y_1 是否大小一致
    if (x_1.size() != y_1.size()) {//如果不一致，输出报错
        std::cerr << "Error: x and y columns have different sizes!" << std::endl;
        return tmpXY;
    }
    // 将 x_1 和 y_1 中的值整合为 QPointF，并存入 QList
    for (size_t i = 0; i < x_1.size(); ++i) {
        QPointF point(x_1[i], y_1[i]);  // 创建 QPointF
        tmpXY.append(point);            // 添加到 QList
        // 打印每个 QPointF 的值
        std::cout << "QPointF(" << point.x() << ", " << point.y() << ")" << std::endl;
    }
    return tmpXY;
}

//执行对launch文件内容，指定变量的修改
void work::modifyWayPointLoader(const QString &filePath, const QString &csvFilePath)
{
    QFile file(filePath); // 打开launch文件

    // 如果文件打开失败，输出错误提示
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for reading:" << file.errorString();
        return;
    }

    // 读取文件内容并解析为DOM文档
    QDomDocument document;
    if (!document.setContent(&file)) {
        qDebug() << "Failed to parse the file into a DOM tree.";
        file.close();
        return;
    }
    file.close(); // 解析完文件内容后可以关闭文件

    // 查找所有 <param> 标签
    QDomNodeList paramElements = document.elementsByTagName("param");
    for (int i = 0; i < paramElements.size(); ++i) {
        QDomElement paramElement = paramElements.at(i).toElement();

        // 判断 param 标签的 name 属性是否是 "path"
        if (paramElement.hasAttribute("name") && paramElement.attribute("name") == "path") {
            // 修改 value 属性，将新的 csv 文件路径设置为 value
            paramElement.setAttribute("value", csvFilePath);
            qDebug() << "Modified path to:" << csvFilePath;
            break; // 找到后就退出循环
        }
    }

    // 将修改后的内容写回文件
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for writing:" << file.errorString();
        return;
    }

    QTextStream out(&file);
    document.save(out, 4); // 将 DOM 文档内容保存回文件
    file.close();
}

void work::read_Launch_WayPointLoader()
{
    //QString launchFile = getFilePath();
    if(flag_launch_WayPointLoader){
        Process_ReadLaunch->start("bash");
        QString processStringBashrc = "source ~/ws/devel/setup.bash";
        //执行命令前先source一下
        Process_ReadLaunch->write(processStringBashrc.toUtf8()+'\n');
        QString processStringReadLaunch = "roslaunch waypoint_updater waypoint_loader_updater.launch";
        //读取保存路径并更新最短跟随路径
        Process_ReadLaunch->write(processStringReadLaunch.toUtf8()+'\n');//
        qDebug() << "执行命令："<< "roslaunch waypoint_updater waypoint_loader_updater.launch";
        //ui->ptb_readPath0->setText("停止读取");
        flag_launch_WayPointLoader=!flag_launch_WayPointLoader;
    }else{
        //杀死进程
        Process_ReadLaunch->write(QString(QChar(0x03)).toUtf8());
        Process_ReadLaunch->kill();
        Process_ReadLaunch->close();
        //重启终端
        Process_ReadLaunch->start("bash");
        qDebug() << "正在关闭 '[读取保存路径并更新最短跟随路径]'进程...";
        // 使用 pkill 来杀死所有 roslaunch 和 roscore 相关进程
        QProcess pkillProcess;
        pkillProcess.start("pkill -f waypoint_loader_updater.launch");  // 杀死与 roslaunch 相关的进程
        pkillProcess.waitForFinished();
        // 检查是否成功关闭了进程
        if(pkillProcess.exitStatus() == QProcess::NormalExit) {
            qDebug() << "'[读取保存路径并更新最短跟随路径]'进程已成功关闭";
        } else {
            qDebug() << "'[读取保存路径并更新最短跟随路径]'进程关闭失败";
        }
        QProcess pkillProcess1;
        pkillProcess1.start("rosnode kill /waypoint_loader_updater");
        pkillProcess1.waitForFinished();
        //ui->ptb_readPath0->setText("路径读取");
        flag_launch_WayPointLoader=!flag_launch_WayPointLoader;
        //qDebug()<<"roscore已关闭";
    }
}

void work::on_work_pbt_reNavigation_clicked()
{
    saveChartAsImage();
}

void work::on_work_pbt_CloseState_clicked()
{
    this->close();
}


void work::on_work_pbt_XinHaoState_clicked(bool checked)
{
    emit signal_Cat1Conncet(checked);
    //判断是否被checked
    if(ui->work_pbt_XinHaoState->isChecked()){//被checked选中
        //打开串口+定时发送数据
        //qDebug() << "toolButton被选中";

    }
    else{//没有被选中（默认状态）
        //关闭串口+关闭定时发送数据
        //qDebug() << "toolButton释放";
    }
}

void work::on_work_pbt_JieKouState_clicked(bool checked)
{
    //根据checked的状态，对roscore进行启动+连接
    emit signal_RosConncet(checked);

}

void work::on_work_pbt_CanDeviceConnetion_clicked(bool checked)
{
    //getCanTransform(1);//开启/关闭产测转换
    if(checked){
        ui->work_pbt_CanDeviceConnetion->setText("测产关闭");
        qDebug() << checked;
        CanSavePathString = getSaveCSVPath();
        InitCSV(CanSavePathString);
        getCanTransform(1);
    }else{
        ui->work_pbt_CanDeviceConnetion->setText("产量监测");
        getCanTransform(0);
        CanSavePathString = "";
    }
}



void work::on_work_pbt_rosKill_clicked()
{
    QProcess pkillProcess1;
    pkillProcess1.start("rosnode kill -a");
    pkillProcess1.waitForFinished();
}




void work::on_work_pbt_recordRosbag_clicked(bool checked)
{
    if(checked){
        QProcess rosbagRecordProcess1;
        RosBagSavePathString = getSaveCSVPath();
        QString rosbagCMDString = QString("rosbag record -a -O %1").arg(RosBagSavePathString);
        rosbagRecordProcess1.start(rosbagCMDString);
        //rosbagRecordProcess1.waitForFinished();
        if(rosbagRecordProcess1.waitForFinished()) {
            QString output = rosbagRecordProcess1.readAllStandardOutput();
            QString errorOutput = rosbagRecordProcess1.readAllStandardError();
            // 打印输出和错误信息
            qDebug() << output;
            qDebug() << errorOutput;
        } else {
            // 处理错误情况
        }
    }
    else{//checked = 0
        QProcess rosbagRecordProcess1;
        QString rosbagCMDString = "pkill -f \"rosbag\"";
        rosbagRecordProcess1.start(rosbagCMDString);
        rosbagRecordProcess1.waitForFinished();
        QMessageBox::information(this, tr("rosbag文件已保存"), tr("文件保存到: %1").arg(RosBagSavePathString));
        RosBagSavePathString = "";//删除历史路径
    }
}


void work::on_work_pbt_mapClear_clicked()
{
    work_clearTractorRealPath();
}

void work::on_pbt_tmpSetPath_clicked(bool checked)
{
    //ChartItem->tmpSetABPath();
}



