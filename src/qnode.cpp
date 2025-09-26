/**
 * @file /src/qnode.cpp
 *
 * @brief Ros communication central!
 *
 * @date February 2011
 **/

/*****************************************************************************
** Includes
*****************************************************************************/

#include <ros/ros.h>
#include <ros/network.h>
#include <string>
#include <std_msgs/String.h>
#include <sstream>
#include "include/qnode.hpp"
#include "QDebug"

/*****************************************************************************
** Namespaces
*****************************************************************************/


namespace ros_qt_demo {

/*****************************************************************************
** Implementation
*****************************************************************************/

QNode::QNode(int argc, char** argv ) :
	init_argc(argc),
	init_argv(argv)
	{}

QNode::~QNode() {
    if(ros::isStarted()) {
      ros::shutdown(); // explicitly needed since we use ros::start();
      ros::waitForShutdown();
    }
	wait();
}

bool QNode::init() {
    ros::init(init_argc,init_argv,"ros_qt_demo_by_SQ");
	if ( ! ros::master::check() ) {
		return false;
	}
	ros::start(); // explicitly needed since our nodehandle is going out of scope.
	ros::NodeHandle n;
	// Add your ros communications here.
	chatter_publisher = n.advertise<std_msgs::String>("chatter", 1000);
    fix_sub=n.subscribe("/fix",10,&QNode::fix_callback,this);
    finalPath_sub=n.subscribe("/final_path",10,&QNode::finalPath_callback,this);
    basePath_sub=n.subscribe("/base_path",10,&QNode::basePath_callback,this);
    vehicle_pose_sub=n.subscribe("/vehicle_pose",1,&QNode::vehiclePose_callback,this);
    velSpeed_sub=n.subscribe("/vel",1,&QNode::velSpeed_callback,this);
    yaw_sub=n.subscribe("/yaw",1,&QNode::yaw_callback,this);
    roll_sub=n.subscribe("/roll",1,&QNode::roll_callback,this);
    pitch_sub=n.subscribe("/pitch",1,&QNode::pitch_callback,this);
    bds_sub=n.subscribe("/bds",1,&QNode::bdsCallback,this);
    lsd_CAN_sub=n.subscribe("/can_fix_fusion",1000,&QNode::lsd_CAN_callback,this);//lsd的产量相关话题


	start();
    return true;
}

bool QNode::shutdownRos()
{
    ros::shutdown();
}



bool QNode::init(const std::string &master_url, const std::string &host_url) {
	std::map<std::string,std::string> remappings;
	remappings["__master"] = master_url;
	remappings["__hostname"] = host_url;
	ros::init(remappings,"class1_ros_qt_demo");
	if ( ! ros::master::check() ) {
		return false;
	}
	ros::start(); // explicitly needed since our nodehandle is going out of scope.
	ros::NodeHandle n;
	// Add your ros communications here.
    //chatter_publisher = n.advertise<std_msgs::String>("chatter", 1000);
    fix_sub=n.subscribe("/fix",1,&QNode::fix_callback,this);
    finalPath_sub=n.subscribe("/final_path",10,&QNode::finalPath_callback,this);
    basePath_sub=n.subscribe("/base_path",10,&QNode::basePath_callback,this);
    vehicle_pose_sub=n.subscribe("/vehicle_pose",1,&QNode::vehiclePose_callback,this);
    velSpeed_sub=n.subscribe("/vel",1,&QNode::velSpeed_callback,this);
    yaw_sub=n.subscribe("/yaw",1,&QNode::yaw_callback,this);
    roll_sub=n.subscribe("/roll",1,&QNode::roll_callback,this);
    pitch_sub=n.subscribe("/pitch",1,&QNode::pitch_callback,this);
    bds_sub=n.subscribe("/bds",1,&QNode::bdsCallback,this);
    lsd_CAN_sub=n.subscribe("/can_fix_fusion",1000,&QNode::lsd_CAN_callback,this);//lsd的产量相关话题
	start();
	return true;
}

//callback of /fix
void QNode::fix_callback(const sensor_msgs::NavSatFix &msg)
{
    emit fix_vel(msg.longitude,msg.latitude,msg.altitude);

    // 1. 提取经纬度和高度
    double lon = msg.longitude;
    double lat = msg.latitude;
    double alt = msg.altitude;
    // 2. 发射信号，供界面更新

    // 3. 添加日志到模型中（可用于view_logging显示）
    if (&logging_model) {
        QString log = QString("GPS Fix - Lon: %1, Lat: %2, Alt: %3")
                        .arg(lon, 0, 'f', 6)
                        .arg(lat, 0, 'f', 6)
                        .arg(alt, 0, 'f', 2);

        QStringList logs = logging_model.stringList();  // 获取当前日志列表
        logs.append(log);                                // 添加新日志
        logging_model.setStringList(logs);              // 设置更新后的日志列表

        emit loggingUpdated();  // 通知界面刷新（比如滚动到底部）
    }
    //    emit fix_vel(msg.status.s)
}

void QNode::vehiclePose_callback(const geometry_msgs::PoseStamped &msg)
{
    float x1 = msg.pose.position.x;
    float y1 = msg.pose.position.y;
    emit GlobalSignal::instance()->gloablSignal_update_vehicle_Pos(x1,y1);
    emit finalPathXY_vel(x1,y1);
}

void QNode::finalPath_callback(const nav_msgs::Path &msg)
{
    float x1 = msg.poses[0].pose.position.x;
    float y1 = msg.poses[0].pose.position.y;
    emit GlobalSignal::instance()->globalSignal_update_finalPathXY(x1,y1);
}

void QNode::basePath_callback(const nav_msgs::Path &msg)
{
    float x1 = msg.poses[0].pose.position.x;
    float y1 = msg.poses[0].pose.position.y;
    emit GlobalSignal::instance()->globalSignal_update_basePathXY(x1,y1);
}

void QNode::velSpeed_callback(const geometry_msgs::TwistStamped &msg)
{
    float linear_x = msg.twist.linear.x;
    float linear_y = msg.twist.linear.y;
    emit velSpeed_vel(linear_x,linear_y);
    // 3. 添加日志到模型中（可用于view_logging显示）
    if (&logging_model) {
        QString log = QString("SpeedX/SpeedY: %1,%2")
                        .arg(linear_x, 0, 'f', 6)
                .arg(linear_y, 0, 'f', 6);

        QStringList logs = logging_model.stringList();  // 获取当前日志列表
        logs.append(log);                                // 添加新日志
        logging_model.setStringList(logs);              // 设置更新后的日志列表

        emit loggingUpdated();  // 通知界面刷新（比如滚动到底部）
    }
}

void QNode::yaw_callback(const nmea_navsat_driver::yaw &msg)
{
    float GNSS_yaw=msg.yaw;
    emit GlobalSignal::instance()->globalSignal_update_yaw(GNSS_yaw);
    emit yaw_vel(GNSS_yaw);

    // 3. 添加日志到模型中（可用于view_logging显示）
    if (&logging_model) {
        QString log = QString("IMU-YAW: %1")
                        .arg(GNSS_yaw, 0, 'f', 6);

        QStringList logs = logging_model.stringList();  // 获取当前日志列表
        logs.append(log);                                // 添加新日志
        logging_model.setStringList(logs);              // 设置更新后的日志列表

        emit loggingUpdated();  // 通知界面刷新（比如滚动到底部）
    }
}

void QNode::roll_callback(const nmea_navsat_driver::roll &msg)
{
    float GNSS_roll=msg.roll;
    emit GlobalSignal::instance()->globalSignal_update_roll(GNSS_roll);
    emit roll_vel(GNSS_roll);

    // 3. 添加日志到模型中（可用于view_logging显示）
    if (&logging_model) {
        QString log = QString("IMU-ROLL: %1")
                        .arg(GNSS_roll, 0, 'f', 6);

        QStringList logs = logging_model.stringList();  // 获取当前日志列表
        logs.append(log);                                // 添加新日志
        logging_model.setStringList(logs);              // 设置更新后的日志列表

        emit loggingUpdated();  // 通知界面刷新（比如滚动到底部）
    }
}

void QNode::pitch_callback(const nmea_navsat_driver::pitch &msg)
{
    float GNSS_pitch=msg.pitch;
    emit GlobalSignal::instance()->globalSignal_update_pitch(GNSS_pitch);
    emit pitch_vel(GNSS_pitch);
    if (&logging_model) {
        QString log = QString("IMU-PITCH: %1")
                        .arg(GNSS_pitch, 0, 'f', 6);

        QStringList logs = logging_model.stringList();  // 获取当前日志列表
        logs.append(log);                                // 添加新日志
        logging_model.setStringList(logs);              // 设置更新后的日志列表

        emit loggingUpdated();  // 通知界面刷新（比如滚动到底部）
    }
}

void QNode::bdsCallback(const nmea_navsat_driver::bds &msg)
{
    float GNSS_bds_num=msg.num;
    float GNSS_bds_status=msg.status;
    emit GlobalSignal::instance()->globalSignal_update_bds(GNSS_bds_num,GNSS_bds_status);
    emit satellite_vel(GNSS_bds_num,GNSS_bds_status);
}

void QNode::lsd_CAN_callback(const std_msgs::String &msg)
{
    QString canString = QString::fromStdString(msg.data);
    emit GlobalSignal::instance()->globalSignal_update_lsd_CAN(canString);
    emit harvestData_CAN(canString);
}





//void QNode::yaw_callback(const nmea_navsat_driver::yaw &msg)
//{
//    float yawValue = msg;
//    emit yaw_vel(yawValue);

//}

void QNode::run() {
    ros::Rate loop_rate(10);
	int count = 0;
	while ( ros::ok() ) {

		std_msgs::String msg;
		std::stringstream ss;
		ss << "hello world " << count;
		msg.data = ss.str();
		chatter_publisher.publish(msg);
        //log(Info,std::string("I sent: ")+msg.data);
		ros::spinOnce();
		loop_rate.sleep();
		++count;
	}
	std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
    Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)
}

void QNode::killRosNode()
{
    QProcess pkillProcess1;
    pkillProcess1.start("rosnode kill -a");
    pkillProcess1.waitForFinished();
}

void QNode::getGNSSTransform(bool &state)
{

    QProcess *Process_GNSSTransform = new QProcess();
    if(state == 1){
        // 启动bash
        Process_GNSSTransform->start("bash");
        // 执行命令前先source一下
        QString processStringBashrc = "source ~/ws/devel/setup.bash";
        Process_GNSSTransform->write(processStringBashrc.toUtf8()+'\n');//执行source命令
        // 执行launch命令 【nmea_serial_driver_posture_pose】
        QString processStringGNSSTransform = "roslaunch nmea_navsat_driver nmea_serial_driver_posture_pose.launch";
        //订阅GNSS信息并发布位置信息
        Process_GNSSTransform->write(processStringGNSSTransform.toUtf8()+'\n');//
//        ui->work_pbt_DeviceConnetion->setText("GNSS关闭");
    }else{//state == 1时关闭roscore
        //杀死进程
        Process_GNSSTransform->write(QString(QChar(0x03)).toUtf8());
        Process_GNSSTransform->kill();
        Process_GNSSTransform->close();
//        //重启终端
//        Process_GNSSTransform->start("bash");
//        qDebug() << "正在关闭 GNSS 转换进程...";
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
        //qDebug()<<"roscore已关闭";
//        ui->work_pbt_DeviceConnetion->setText("GNSS读取");
    }
}

void QNode::savePath_Launch(bool &state)//flag_launch_savePath
{
    QProcess *Process_savePathLaunch = new QProcess();
    //QString launchFile = getFilePath();
    if(state == 0){  state=1;//标志位取反
        Process_savePathLaunch->start("bash");
        QString processStringBashrc = "source ~/ws/devel/setup.bash";
        //执行命令前先source一下
        Process_savePathLaunch->write(processStringBashrc.toUtf8()+'\n');
        QString processStringReadLaunch = "roslaunch nmea_navsat_driver nmea_serial_driver_save_path.launch";
        //读取保存路径并更新最短跟随路径
        Process_savePathLaunch->write(processStringReadLaunch.toUtf8()+'\n');//
        qDebug() << "执行命令："<< processStringReadLaunch;
        //ui->ptb_SavePath->setText("停止记录");
    }else{      state=0;//标志位取反
        //杀死进程
        Process_savePathLaunch->write(QString(QChar(0x03)).toUtf8());
        Process_savePathLaunch->kill();
        Process_savePathLaunch->close();
        qDebug() << "正在关闭 '[路径记录nmea_serial_driver_save_path]'进程...";
        // 使用 pkill 来杀死所有 roslaunch 和 roscore 相关进程
        QProcess pkillProcess;
        pkillProcess.start("pkill -f nmea_serial_driver_save_path.launch");  // 杀死与 roslaunch 相关的进程
        pkillProcess.waitForFinished();
        // 检查是否成功关闭了进程
        if(pkillProcess.exitStatus() == QProcess::NormalExit) {
            qDebug() << "'[路径记录nmea_serial_driver_save_path]'进程已成功关闭";
        } else {
            qDebug() << "'[路径记录nmea_serial_driver_save_path]'进程关闭失败";
        }
        //ui->ptb_SavePath->setText("开始记录");
        //qDebug()<<"roscore已关闭";
    }
}

// launch:waypoint_loader_updater
void QNode::getLoadPath(bool &state)
{
    qDebug() << "loadPathLaunchState_: " << QString::number(state);
    QProcess *Process_ReadLaunch = new QProcess();
    //QString launchFile = getFilePath();
    if(state == 0){
        Process_ReadLaunch->start("bash");
        QString processStringBashrc = "source ~/ws/devel/setup.bash";
        //执行命令前先source一下
        Process_ReadLaunch->write(processStringBashrc.toUtf8()+'\n');
        QString processStringReadLaunch = "roslaunch waypoint_updater waypoint_loader_updater.launch";
        //读取保存路径并更新最短跟随路径
        Process_ReadLaunch->write(processStringReadLaunch.toUtf8()+'\n');//
        qDebug() << "执行命令："<< "roslaunch waypoint_updater waypoint_loader_updater.launch";
        //ui->ptb_readPath0->setText("停止读取");
        state=!state;
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
        //ui->ptb_readPath0->setText("路径读取");
        state=!state;
        //qDebug()<<"roscore已关闭";
    }
}

void QNode::autoNavigation(bool &state)
{
    qDebug() << "pathPersuitState_: " << QString::number(state);
    QProcess *Process_ReadLaunch = new QProcess;
    if(state == 0){
        Process_ReadLaunch->start("bash");
        QString processStringBashrc = "source ~/ws/devel/setup.bash";
        //执行命令前先source一下
        Process_ReadLaunch->write(processStringBashrc.toUtf8()+'\n');
        QString processStringReadLaunch = "roslaunch turn_on_wheeltec_robot turn_on_persuit_serial_robot.launch";
        //读取保存路径并更新最短跟随路径
        Process_ReadLaunch->write(processStringReadLaunch.toUtf8()+'\n');//
        qDebug() << "执行命令："<< "roslaunch turn_on_wheeltec_robot turn_on_persuit_serial_robot.launch";
        //ui->ptb_readPath0->setText("结束跟踪");
        state=!state;
    }else{
        //杀死进程
        Process_ReadLaunch->write(QString(QChar(0x03)).toUtf8());
        Process_ReadLaunch->kill();
        Process_ReadLaunch->close();
        //重启终端
        Process_ReadLaunch->start("bash");
        qDebug() << "正在关闭 路径跟踪进程...";
        // 使用 pkill 来杀死所有 roslaunch 和 roscore 相关进程
        QProcess pkillProcess;
        pkillProcess.start("pkill -f turn_on_persuit_serial_robot.launch");  // 杀死与 roslaunch 相关的进程
        pkillProcess.waitForFinished();
        // 检查是否成功关闭了进程
        if(pkillProcess.exitStatus() == QProcess::NormalExit) {
            qDebug() << "'[路径跟踪进程已成功关闭]'";
        } else {
            qDebug() << "'[路径跟踪进程关闭失败]'";
        }
        //ui->ptb_readPath0->setText("开启跟踪");
        state=!state;
        //qDebug()<<"roscore已关闭";
    }
}

void QNode::getCanTransform(bool &state)
{
    QProcess Process_CanTransform;
    if(state == 0){state = !state;
        Process_CanTransform.start("bash");
        QString processStringBashrc = "source ~/ws/devel/setup.bash";
        //执行命令前先source一下
        Process_CanTransform.write(processStringBashrc.toUtf8()+'\n');
        QString processStringCanTransform = "roslaunch ros_can ros_can.launch";
        //订阅Can信息并发布位置信息
        Process_CanTransform.write(processStringCanTransform.toUtf8()+'\n');//
    }else{state = !state;//state == 1时关闭ros_can的launch文件
        //杀死进程
        Process_CanTransform.write(QString(QChar(0x03)).toUtf8());
        Process_CanTransform.kill();
        Process_CanTransform.close();
        //重启终端
        Process_CanTransform.start("bash");
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

// controlLaunch文件用于控制launch文件的状态
// 传入参数：启停状态(state)，launch文件名(launchName)
// 注意：务必确保包名和文件名一致
void QNode::controlLaunch(bool &state,QString launchName){
    if(Process_Launch == NULL){
        Process_Launch = new QProcess();
    }
    if(state == 0){
        //启动终端
        Process_Launch->start("bash");
        //执行命令前先source一下
        QString processStringBashrc = "source ~/ws/devel/setup.bash";
        Process_Launch->write(processStringBashrc.toUtf8()+'\n');
        //执行launch文件控制
        QString process_launchName = "roslaunch " + launchName + " " +launchName+".launch";
        qDebug() << "执行命令：" << process_launchName;//打印输出，检查状态
        Process_Launch->write(process_launchName.toUtf8()+'\n');
        state=!state;
    }else{
        // 使用 pkill 来杀死所有 roslaunch 和 roscore 相关进程
        QProcess pkillProcess;
        QString string_pkill = "pkill -f "+launchName+".launch";
        pkillProcess.start(string_pkill.toUtf8());  // 杀死与 roslaunch 相关的进程
        pkillProcess.waitForFinished();
        // 检查是否成功关闭了进程
        if(pkillProcess.exitStatus() == QProcess::NormalExit) {
            qDebug() << launchName+"进程已成功关闭";
        } else {
            qDebug() << launchName+"进程关闭失败";
        }
        state=!state;
    }
}

void QNode::sub_image(QString topic_Name)
{
    ros::NodeHandle n;
    image_transport::ImageTransport it_(n);
    image_sub = it_.subscribe(topic_Name.toStdString(),1000,&QNode::image_callback,this);
}

void QNode::image_callback(const sensor_msgs::ImageConstPtr &msg)
{
    cv_bridge::CvImagePtr cv_ptr;
    cv_ptr=cv_bridge::toCvCopy(msg,msg->encoding);
    QImage im=Mat2QImage(cv_ptr->image);
    emit image_val(im);
}

QImage QNode::Mat2QImage(cv::Mat const& src)
{
  QImage dest(src.cols, src.rows, QImage::Format_ARGB32);

  const float scale = 255.0;

  if (src.depth() == CV_8U) {
    if (src.channels() == 1) {
      for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
          int level = src.at<quint8>(i, j);
          dest.setPixel(j, i, qRgb(level, level, level));
        }
      }
    } else if (src.channels() == 3) {
      for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
          cv::Vec3b bgr = src.at<cv::Vec3b>(i, j);
          dest.setPixel(j, i, qRgb(bgr[2], bgr[1], bgr[0]));
        }
      }
    }
  } else if (src.depth() == CV_32F) {
    if (src.channels() == 1) {
      for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
          int level = scale * src.at<float>(i, j);
          dest.setPixel(j, i, qRgb(level, level, level));
        }
      }
    } else if (src.channels() == 3) {
      for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
          cv::Vec3f bgr = scale * src.at<cv::Vec3f>(i, j);
          dest.setPixel(j, i, qRgb(bgr[2], bgr[1], bgr[0]));
        }
      }
    }
  }

  return dest;
}




void QNode::log( const LogLevel &level, const std::string &msg) {
	logging_model.insertRows(logging_model.rowCount(),1);
	std::stringstream logging_model_msg;
	switch ( level ) {
		case(Debug) : {
				ROS_DEBUG_STREAM(msg);
				logging_model_msg << "[DEBUG] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Info) : {
				ROS_INFO_STREAM(msg);
				logging_model_msg << "[INFO] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Warn) : {
				ROS_WARN_STREAM(msg);
				logging_model_msg << "[INFO] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Error) : {
				ROS_ERROR_STREAM(msg);
				logging_model_msg << "[ERROR] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Fatal) : {
				ROS_FATAL_STREAM(msg);
				logging_model_msg << "[FATAL] [" << ros::Time::now() << "]: " << msg;
				break;
		}
	}
	QVariant new_row(QString(logging_model_msg.str().c_str()));
	logging_model.setData(logging_model.index(logging_model.rowCount()-1),new_row);
	Q_EMIT loggingUpdated(); // used to readjust the scrollbar
}

}  // namespace class1_ros_qt_demo
