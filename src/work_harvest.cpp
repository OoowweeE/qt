#include "work_harvest.h"
#include "ui_work_harvest.h"

work_harvest::work_harvest(QWidget *parent,QNode *RosNode,SQChart *pathData) :
    QWidget(parent),
    ui(new Ui::work_harvest),//变量初始化
    RosNode_(RosNode),
    ChartItem_(pathData)
{
    ui->setupUi(this);
    ui->HarvestView->setChart(ChartItem_->chart_harvest_XY);
    ui->HarvestView->setBackgroundBrush(Qt::white);//Qt:White
    /*1 数据更新类信号与槽 */
    // 更新
    connect(RosNode_,&QNode::harvestData_CAN,this,&work_harvest::UpdateHarvestData_CAN);
    connect(RosNode_,&QNode::finalPathXY_vel,this,&work_harvest::UpdateNaviXY);
    //更新角度
    connect(RosNode_,&QNode::yaw_vel,this,&work_harvest::UpdateNavi_yaw);
}

work_harvest::~work_harvest()
{
    delete ui;
}

void work_harvest::UpdateHarvestData_CAN(QString canString)
{
    qDebug()<<canString;//打印出来这个数据
    YieldData::instance().String2Struct_yield_CAN(canString); //将原始数据解析至结构体中
    getCanString2Qt(canString);                               //解析数据至Qt界面
    if(!CanSavePathString.isEmpty()){ // 保存数据
        YieldData::instance().CanSaveToCSV(CanSavePathString);
    }
}

void work_harvest::UpdateNavi_yaw(float yaw)
{
    harvest_yaw_ = yaw;
}
//更新产量热力地图
void work_harvest::UpdateNaviXY(float x,float y)
{
    float tmp = can_actual_per_Acre_Yield;
//    if(0<tmp&&tmp<250){
//        ChartItem_->setSplineSeriesPen(ChartItem_->tractorRealPathSeries_harvest,
//                                       Qt::SolidLine, 20, QColor(QColor(QRgb(0xFF0000))));
//    }else if(250<=tmp&&tmp<500){
//        ChartItem_->setSplineSeriesPen(ChartItem_->tractorRealPathSeries_harvest,
//                                       Qt::SolidLine, 20, QColor(QColor(QRgb(0xFF6B6B))));
//    }else if(500<=tmp&&tmp<750){
//        ChartItem_->setSplineSeriesPen(ChartItem_->tractorRealPathSeries_harvest,
//                                       Qt::SolidLine, 20, QColor(QColor(QRgb(0xFFA500))));
//    }else if(750<=tmp&&tmp<1000){
//        ChartItem_->setSplineSeriesPen(ChartItem_->tractorRealPathSeries_harvest,
//                                       Qt::SolidLine, 20, QColor(QColor(QRgb(0xFFFF00))));
//    }else if(1000<=tmp&&tmp<1250){
//        ChartItem_->setSplineSeriesPen(ChartItem_->tractorRealPathSeries_harvest,
//                                       Qt::SolidLine, 20, QColor(QColor(QRgb(0x90EE90))));
//    }else if(tmp>=1250){
//        ChartItem_->setSplineSeriesPen(ChartItem_->tractorRealPathSeries_harvest,
//                                       Qt::SolidLine, 20, QColor(QColor(QRgb(0x006400))));
//    }else{
//        ChartItem_->setSplineSeriesPen(ChartItem_->tractorRealPathSeries_harvest,
//                                       Qt::SolidLine, 20, QColor(QColor(Qt::transparent)));
//    }
    ChartItem_->addPointWithYieldColor(QPointF(-y,x),tmp);
    ChartItem_->setTractorRealPathSeriesCar(-y,x,harvest_yaw_);
    //ChartItem_->tractorRealPathSeries_harvest->append(-y,x);
    //ChartItem_->tractorRealPathSeries_harvest->append(y,x);
}
// 计算当前单位面积产量



void work_harvest::getCanString2Qt(QString canString)
{
    QStringList canStringList = canString.split(",");
    QString can_ID = canStringList[0];
    QString can_Data = canStringList[1];
    if(can_ID == "419366615"){
        float can_totalYield = YieldData::HarvestID1_totalYield(can_Data);
        globalValue::ANetData.dangqianchanliang = can_totalYield;
        qDebug()<< "-----------------"<<QString::number(can_totalYield, 'f', 3);
        ui->work_label_totalYield->setText(QString::number(can_totalYield, 'f', 3));
        float speedXY = YieldData::instance().canData.speed;
        can_actual_per_Acre_Yield = can_totalYield / 5 /speedXY * 666.67;
        globalValue::ANetData.shishimuchan = can_actual_per_Acre_Yield;
        ui->work_label_realTimeYield->setText(QString::number(can_actual_per_Acre_Yield, 'f', 3));
    }else if(can_ID == "419365450"){
        float can_grainMoisture = YieldData::HarvestID2_grainMoisture(can_Data);
        globalValue::ANetData.guwuhanshuilv = can_grainMoisture;
        ui->work_label_grainMoisture->setText(QString::number(can_grainMoisture, 'f', 3));
    }else if(can_ID == "419366614"){
        float can_grainFlowRate = YieldData::HarvestID3_grainFlowRate(can_Data);
        globalValue::ANetData.guwuliuliang = can_grainFlowRate;
        ui->work_label_grainFlowRate->setText(QString::number(can_grainFlowRate, 'f', 6));
    }
}

void work_harvest::on_work_pbt_CanDeviceConnetion_clicked(bool checked)
{
    //getCanTransform(1);//开启/关闭产测转换
    if(checked){
        ui->work_pbt_CanDeviceConnetion->setText("测产关闭");
        qDebug() << checked;
        //CanSavePathString = getSaveCSVPath();
        QString defaultDirectory;
        //CanSavePathString = QFileDialog::getOpenFileName(nullptr, "选择文件", defaultDirectory, "所有文件 (*)");
        CanSavePathString = QFileDialog::getOpenFileName(nullptr, "选择文件", "", "所有文件 (*)");
        YieldData::InitCSV(CanSavePathString);
//        getCanTransform(1);
    }else{
        ui->work_pbt_CanDeviceConnetion->setText("产量监测");
//        getCanTransform(0);
        CanSavePathString = "";
    }
    RosNode_->getCanTransform(SysSetting::instance().harvestCANLaunchState_);
}

void work_harvest::on_work_pbt_recordRosbag_clicked(bool checked)
{
    if(checked){
        QProcess rosbagRecordProcess1;
        RosBagSavePathString = QFileDialog::getOpenFileName(nullptr, "选择文件", "", "所有文件 (*)");
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
