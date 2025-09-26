#include "work_navi.hpp"
#include "ui_work_navi.h"

#include <ui_work_navi.h>
#include <work_navi.hpp>


work_navi::work_navi(QWidget *parent,QNode *RosNode,SQChart *pathData) :
    QWidget(parent),
    ui(new Ui::work_navi)
{
    ui->setupUi(this);
    RosNode_ = RosNode;  //指向传入进来RosNode
    ChartItem_ = pathData;//指向传入进来的pathData
    //20241206
    ui->work_chartView->setChart(ChartItem_->chart_navi_XY);
    ui->work_chartView->setBackgroundBrush(Qt::white);//Qt:White
    //RosNode->init();

    //绑定qnode数据变化信号与work_navi改变ui数据展示槽函数
    //connect(GlobalSignal::instance(), &GlobalSignal::globalSignal_update_lsd_CAN,this, &work::slot_update_lsd_CAN);
    //connect(GlobalSignal::instance(), &GlobalSignal::gloablSignal_update_vehicle_Pos,this, &work_navi::UpdateNaviXY);
    //connect(ui->pathplan_ptb_closeWindow,&QPushButton::clicked,this,&pathPlan::close);

    /*1 数据更新类信号与槽 */
    // 更新GNSS的三维数据
    connect(RosNode_,&QNode::fix_vel,this,&work_navi::UpdateNaviGNSS);
    // 更新二维XY数据
    connect(RosNode_,&QNode::finalPathXY_vel,this,&work_navi::UpdateNaviXY);
    // 更新IMU姿态数据
    connect(RosNode_,&QNode::yaw_vel,this,&work_navi::UpdateNavi_yaw);
    connect(RosNode_,&QNode::roll_vel,this,&work_navi::UpdateNavi_roll);
    connect(RosNode_,&QNode::pitch_vel,this,&work_navi::UpdateNavi_pitch);
    // 更新速度数据
    connect(RosNode_,&QNode::velSpeed_vel,this,&work_navi::UpdateNavi_speed);
    // 更新卫星状态和更新卫星数
    connect(RosNode_,&QNode::satellite_vel,this,&work_navi::UpdateNavi_satellite);
    /*1 数据更新类信号与槽 */


    /*2 按钮控制类槽函数 */
    // 点击work_pbt_rosKill后杀死所有rosnode节点（相当于全部杀死了）
    connect(ui->work_pbt_rosKill,&QPushButton::clicked,this,&work_navi::killRosNode);
    // 点击work_pbt_mapClear后清空农机行驶轨迹和规划路径，并重新标定原点
    connect(ui->work_pbt_mapClear,&QPushButton::clicked,this,&work_navi::ClearPath);
    // 点击放大缩小后，chart视图缩放
    connect(ui->work_pbt_mapEnlarge,&QPushButton::clicked,this,&work_navi::map_Zoom_In);
    connect(ui->work_pbt_mapReduce,&QPushButton::clicked,this,&work_navi::map_Zoom_Out);
    // 点击work_pbt_renew后，chart视图重置初始化
    connect(ui->work_pbt_renew,&QPushButton::clicked,this,&work_navi::map_Reset);
    // 点击work_pbt_save2local后，将chartView视图保存至本地
    connect(ui->work_pbt_save2local,&QPushButton::clicked,this,&work_navi::map_savePic2local);
}

work_navi::~work_navi()
{
    delete ui;
}
// 更新GNSS的三维数据
void work_navi::UpdateNaviGNSS(float longitude, float latitude, float altitude)
{

    ui->work_label_longtitudeValue->setText(QString::number(longitude,'f',6));
    ui->work_label_latitudeValue->setText(QString::number(latitude,'f',6));
    ui->work_label_altitudeValue->setText(QString::number(altitude,'f',6));
    globalValue::ANetData.longitude = longitude;
    globalValue::ANetData.latitude = latitude;
    globalValue::ANetData.altitude = altitude;
    //qDebug() << "test";
}
// 更新二维XY数据
void work_navi::UpdateNaviXY(float x, float y)
{
    ui->work_label_finalPathX->setText(QString::number(x,'f',6));
    ui->work_label_finalPathY->setText(QString::number(y,'f',6));

    UpdateChart_XY_jiaodu(x,-y,ui->work_label_yawValue->text().toDouble());//Chart中农机位置
}
// 更新IMU姿态数据:yaw
void work_navi::UpdateNavi_yaw(float yaw)
{
    ui->work_label_yawValue->setText(QString::number(yaw,'f',6));
    globalValue::ANetData.yaw = yaw;
}
// 更新IMU姿态数据:roll
void work_navi::UpdateNavi_roll(float roll)
{
    ui->work_label_rollValue->setText(QString::number(roll,'f',6));

}
// 更新IMU姿态数据:pitch
void work_navi::UpdateNavi_pitch(float pitch)
{
    ui->work_label_pitchValue->setText(QString::number(pitch,'f',6));
    globalValue::ANetData.pitch = pitch;
}
// 更新速度数据
void work_navi::UpdateNavi_speed(float speedX,float speedY)
{
    ui->work_label_linearXValue->setText(QString::number(speedX,'f',6));
    ui->work_label_linearYValue->setText(QString::number(speedY,'f',6));
    float speedXY = sqrt(speedX*speedX+speedY*speedY);
    ui->work_label_linearXYValue->setText(QString::number(speedXY,'f',6));

    globalValue::ANetData.speed = speedXY;
}

// 更新卫星状态和更新卫星数
void work_navi::UpdateNavi_satellite(int num, int state)
{
    ui->work_label_satelliteNum->setText(QString::number(num));
    if(state == 0){
        ui->work_label_satelliteState->setText("float");
    }else if(state == 4){
        ui->work_label_satelliteState->setText("fixed");
    }else
        ui->work_label_satelliteState->setText("None");
}
// 更新农机位置及车头前进方向
void work_navi::UpdateChart_XY_jiaodu(float x,float y,float jiaodu){
    ChartItem_->tractorRealPathSeries->append(y,x);          // 更新序列
    ChartItem_->setTractorRealPathSeriesCar(y,x,jiaodu);     // 更新农机图标及角度
    ChartItem_->tractorRealPathSeries_pathPlan->append(y,x); // 更新路径规划页面-实际运动轨迹


}

// btn1-1:map_Reset | 重置视图
void work_navi::map_Reset(){
    ChartItem_->resetView();
}

// btn1-2:map_save2local | 保存视图（保存视图至本地）
void work_navi::map_savePic2local(){
    ui->work_chartView->saveAsImage();
}

// btn1-3:map_Zoom_In | 放大视图
void work_navi::map_Zoom_In(){
    ChartItem_->chart_navi_XY->zoom(1.1);
}

// btn1-4:map_Zoom_Out| 缩小视图
void work_navi::map_Zoom_Out(){
    ChartItem_->chart_navi_XY->zoom(0.9);
}

// btn1-5：killRos
void work_navi::killRosNode(){
    RosNode_->killRosNode();
}

// btn1-6:clearPath // 清除轨迹+重新标定原点
void work_navi::ClearPath(){
    ChartItem_->clearRealPath();
}

/* btn2:GNSS读取、路径读取、路径跟踪、XY转经纬 */
void work_navi::ReadGNSS(bool *state)
{
    if(*state == 1){ *state = 0;//重置标志位
        ui->work_pbt_GNSSConnetion->setText("GNSS读取");
    }
    else{ *state = 1;//重置标志位
        ui->work_pbt_GNSSConnetion->setText("GNSS关闭");
        ChartItem_->clearRealPath();
    }
    RosNode_->getGNSSTransform(*state);//根据当前的state执行GNSS转换
}

void work_navi::on_work_pbt_GNSSConnetion_clicked()
{
    // 里面会自动对标志位gnssLaunchState_进行置位
    ReadGNSS(&(SysSetting::instance().gnssLaunchState_));
    //qDebug() << QString::number(SysSetting::instance().gnssLaunchState_);
}

void work_navi::on_work_pbt_loadPath_clicked()
{
    if((SysSetting::instance().loadPathLaunchState_) == 0){ // 可以路径读取
        QString defaultDirectory = "/home/nvidia/ws/src/waypoint_loader/waypoints";
        QString csvFilePath = QFileDialog::getOpenFileName(nullptr, "选择文件", defaultDirectory, "所有文件 (*)");
        // 获取csv文件的内容，保存为QPointF点ChartItem_->CsvPointF
        ChartItem_->CsvPointF = GlobalSignal::instance()->ReadCSV_XY(csvFilePath);
        // 将点输出到界面上(读取csv文件的点，然后写入到tractorPlanPathSeries中）
        ChartItem_->getHistoryPoint_DotLine(ChartItem_->CsvPointF);//将路径轨迹显示出来
        // 更新ui界面控件
        ui->work_pbt_loadPath->setText("路径清除");
        ui->work_pbt_loadPath->setChecked(1);
    }else{//loadPathLaunchState_ = 1 // 路径已读
        // 清空『路径规划』路线
        ChartItem_->tractorPlanPathSeries->clear();
        ChartItem_->clearRealPath();// 把农机轨迹页清除
        // 更新ui界面控件
        ui->work_pbt_loadPath->setText("路径跟随");
        ui->work_pbt_loadPath->setChecked(0);
    }
    // 执行launch命令roslaunch waypoint_updater waypoint_loader_updater.launch
    RosNode_->getLoadPath(SysSetting::instance().loadPathLaunchState_);//执行launch文件
}

void work_navi::on_work_pbt_AutoNavigation_clicked()
{
    if((SysSetting::instance().pathPersuitState_) == 0){ // 可以路径读取
        // 更新ui界面控件
        ui->work_pbt_AutoNavigation->setText("结束跟踪");
        ui->work_pbt_AutoNavigation->setChecked(1);
    }else{//loadPathLaunchState_ = 1 // 路径已读
        // 更新ui界面控件
        ui->work_pbt_AutoNavigation->setText("开始跟踪");
        ui->work_pbt_AutoNavigation->setChecked(0);
    }
    // 执行launch命令"roslaunch turn_on_wheeltec_robot turn_on_persuit_serial_robot.launch"
    RosNode_->autoNavigation(SysSetting::instance().pathPersuitState_);//执行launch文件
}
