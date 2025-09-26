/**
 * @file /include/class1_ros_qt_demo/qnode.hpp
 *
 * @brief Communications central!
 *
 * @date February 2011
 **/
/*****************************************************************************
** Ifdefs
*****************************************************************************/

#ifndef class1_ros_qt_demo_QNODE_HPP_
#define class1_ros_qt_demo_QNODE_HPP_

/*****************************************************************************
** Includes
*****************************************************************************/

// To workaround boost/qt4 problems that won't be bugfixed. Refer to
//    https://bugreports.qt.io/browse/QTBUG-22829
#ifndef Q_MOC_RUN
#include <ros/ros.h>
#endif
#include <string>
#include <QThread>
#include <QStringListModel>
#include <std_msgs/Float32.h>
//#include <sensor_msgs/NavSatFix>
#include <sensor_msgs/NavSatFix.h>
#include <geometry_msgs/Point.h>
#include <nav_msgs/Path.h>
#include <std_msgs/String.h>
#include "nmea_navsat_driver/yaw.h"
#include "nmea_navsat_driver/pitch.h"
#include "nmea_navsat_driver/roll.h"
#include "nmea_navsat_driver/bds.h"
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TwistStamped.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <QImage>

#include "globalsignal.h"
#include <QRegularExpression>
/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace ros_qt_demo {

/*****************************************************************************
** Class
*****************************************************************************/

class QNode : public QThread {
    Q_OBJECT
public:
	QNode(int argc, char** argv );
	virtual ~QNode();
	bool init();
    bool shutdownRos();
	bool init(const std::string &master_url, const std::string &host_url);
	void run();
    void killRosNode();//杀死所有的node节点
    //是否执行【GNSS转换读取】命令
    void getGNSSTransform(bool &state);//roslaunch nmea_navsat_driver nmea_serial_driver_posture_pose.launch
    void savePath_Launch(bool &state);
    void getLoadPath(bool &state);
    void autoNavigation(bool &state);
    void getCanTransform(bool &state);//roslaunch ros_can ros_can.launch
    void controlLaunch(bool &state, QString launchName);
    void sub_image(QString topic_Name);
    QImage Mat2QImage(const cv::Mat &src);

    QProcess *Process_GNSSTransform;


	/*********************
	** Logging
	**********************/
	enum LogLevel {
	         Debug,
	         Info,
	         Warn,
	         Error,
	         Fatal
	 };

	QStringListModel* loggingModel() { return &logging_model; }
	void log( const LogLevel &level, const std::string &msg);


Q_SIGNALS:
	void loggingUpdated();
    void rosShutdown();
    void fix_vel(float,float,float);
    void basePath_vel(float,float);
    void finalPathXY_vel(float,float);
    void yaw_vel(float);
    void roll_vel(float);
    void pitch_vel(float);
    void velSpeed_vel(float,float);
    void satellite_vel(int,int);
    void harvestData_CAN(QString canString);
    void image_val(QImage);




private:
	int init_argc;
	char** init_argv;
	ros::Publisher chatter_publisher;
    ros::Subscriber fix_sub;//经度、纬度、高程信息
    ros::Subscriber finalPath_sub;//局部路径
    ros::Subscriber basePath_sub;//全局路径
    ros::Subscriber vehicle_pose_sub;//实际坐标

    ros::Subscriber yaw_sub;  // 航向角
    ros::Subscriber roll_sub; // 横滚角
    ros::Subscriber pitch_sub;// 俯仰角
    ros::Subscriber bds_sub;// 卫星状态 + 卫星个数
    ros::Subscriber velSpeed_sub;//速度
    ros::Subscriber lsd_CAN_sub;//达哥的产量相关的数据
    image_transport::Subscriber image_sub;


    void fix_callback(const sensor_msgs::NavSatFix &msg);// callback of zuobiao

    void vehiclePose_callback(const geometry_msgs::PoseStamped &msg);
    void finalPath_callback(const nav_msgs::Path &msg);
    void basePath_callback(const nav_msgs::Path &msg);
    //void yaw_callback(const nmea_navsat_driver::yaw &msg);
    void velSpeed_callback(const geometry_msgs::TwistStamped &msg);

    void yaw_callback(const nmea_navsat_driver::yaw &msg);
    void roll_callback(const nmea_navsat_driver::roll &msg);
    void pitch_callback(const nmea_navsat_driver::pitch &msg);
    void bdsCallback(const nmea_navsat_driver::bds &msg);

    void lsd_CAN_callback(const std_msgs::String &msg);
    
    void image_callback(const sensor_msgs::ImageConstPtr &msg);//图像话题回调函数


    QStringListModel logging_model;

    QProcess *Process_Launch;
};




}  // namespace class1_ros_qt_demo

#endif /* class1_ros_qt_demo_QNODE_HPP_ */
