#ifndef QRVIZ_HPP
#define QRVIZ_HPP

#include <QObject>
#include <QVBoxLayout>
#include <OgreColourValue.h>

//包含ros及rViz组件库头文件（记得在cmakelist中也添加相关内容）
#include <ros/ros.h>
#include <rviz/visualization_manager.h>
#include <rviz/render_panel.h>
#include <rviz/display.h>
#include <rviz/tool_manager.h>
#include <rviz/tool.h>
#include <rviz/image/image_display_base.h>


/*****************************************************************************
** Namespaces
*****************************************************************************/
namespace class1_ros_qt_demo {
}
/*****************************************************************************
** Class
*****************************************************************************/
class qrviz
{
public:
  //构造函数通过参数传入界面对象指针从而通过该指针添加widget
  qrviz(QVBoxLayout* layout);


  /******************************************
  ** RViz全局设置方法
  *******************************************/
  void SetFixedFrame(QString FrameName);
  void SetPanelBackgroundColor(QColor color);


  /******************************************
  ** 创建Display对象方法
  *******************************************/
  rviz::Display* CreateDisplay(const QString& class_lookup_name, const QString& name);


  /******************************************
  ** 修改Display对象属性方法
  *******************************************/
  void ModifyGridAttribute(rviz::Display* Grid, int CellCount, QColor GridColor, bool Enabled);
  void ModifyTFAttribute(rviz::Display* TF, bool Enabled);
  void ModifyLaserScanAttribute(rviz::Display* LaserScan, QString LaserTopic, QString LaserRenderStyle, double LaserPointSize, bool Enabled);
  void ModifyPointCloud2Attribute(rviz::Display* PointCloud2, QString PointCloud2Topic, QString PointCloud2RenderStyle, double PointCloud2PointSize, bool Enabled);
  void ModifyRobotModelAttribute(rviz::Display* RobotModel, bool Enabled);
  void ModifyMapAttribute(rviz::Display* Map, QString MapTopic, QString ColorScheme, bool Enabled);
  void ModifyPathAttribute(rviz::Display* Path, QString PathTopic, QColor PathColor, bool Enabled);
  void ModifyPointStampedAttribute(rviz::Display* PointStamped, QString PointStampedTopic, QColor PointStampedColor, bool Enabled);
  void ModifyMarkerAttribute(rviz::Display* Marker, QString MarkerTopic, bool Enabled);
  void ModifyMarkerArrayAttribute(rviz::Display* MarkerArray, QString MarkerArrayTopic, bool Enabled);
  void ModifyImageAttribute(rviz::Display* Image, QString ImageTopic, bool Enabled);
  void ModifyPoseAttribute(rviz::Display* Pose, QString PoseTopic, QColor PoseColor, bool Enabled);
  void ModifyBoundingBoxArrayAttribute(rviz::Display* BoundingBoxArray, QString BoundingBoxArrayTopic, bool Enabled);


  /******************************************
  ** 删除Display对象方法
  *******************************************/
  void DeleteDisplayLayer(rviz::Display* layer);


  /******************************************
  ** 使用RViz工具方法
  *******************************************/
  void SetInitialPose();
  void SetGoalPose();
  void PublishPoint();
private:
  /*
    VisualizationManager
    ├── RenderPanel (3D显示容器)-------------------------视图view
    └── ToolManager (工具管理)---------------------------控制control
  */
  rviz::RenderPanel* render_panel;      //声明 负责在UI中显示3D视图（创建rviz显示容器）
  rviz::VisualizationManager* manager;  //声明RViz中央管理器类对象：管理所有显示元素和 RViz 的可视化配置。
  rviz::ToolManager* toolManager;       //声明RViz工具管理器类对象

};

  // namespace class1_ros_qt_demo

#endif // QRVIZ_HPP
