#ifndef WORK_OBSTACLE_H
#define WORK_OBSTACLE_H

#include <QWidget>
#include <QtGui>
#include <QMessageBox>
#include <QDebug>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QMessageBox>

#include "qnode.hpp"
#include "addlayer.h"
#include "mypalette.h"
#include "qrviz.hpp"
#include <jsk_recognition_msgs/BoundingBoxArray.h>

#include "SysSetting.hpp"

using namespace ros_qt_demo;
namespace Ui {
class work_Obstacle;
}

class work_Obstacle : public QWidget
{
    Q_OBJECT

public:
    explicit work_Obstacle(
            int argc=0,
            char** argv=nullptr,
            QWidget *parent = nullptr,
            QNode *RosNode = nullptr
    );
    ~work_Obstacle();
//

    void ReadSetting();     // 读取历史操作和数据
    void WriteSettings();   // 保存相关操作和数据

    /******************************************
    ** 向UI界面添加新图层相关控件方法
    *******************************************/
    QTreeWidgetItem* add_TreeWidgetItem_Grid(QTreeWidget* treeWidget, QString& displayName);
    QTreeWidgetItem* add_TreeWidgetItem_TF(QTreeWidget* treeWidget, QString& displayName);
    QTreeWidgetItem* add_TreeWidgetItem_LaserScan(QTreeWidget* treeWidget, QString& displayName, QString& topicName);
    QTreeWidgetItem* add_TreeWidgetItem_PointCloud2(QTreeWidget* treeWidget, QString& displayName, QString& topicName);
    QTreeWidgetItem* add_TreeWidgetItem_RobotModel(QTreeWidget* treeWidget, QString& displayName);
    QTreeWidgetItem* add_TreeWidgetItem_Map(QTreeWidget* treeWidget, QString& displayName, QString& topicName);
    QTreeWidgetItem* add_TreeWidgetItem_Path(QTreeWidget* treeWidget, QString& displayName, QString& topicName, QColor color = QColor(25, 255, 0));
    QTreeWidgetItem* add_TreeWidgetItem_PointStamped(QTreeWidget* treeWidget, QString& displayName, QString& topicName);
    QTreeWidgetItem* add_TreeWidgetItem_Marker(QTreeWidget* treeWidget, QString& displayName, QString& topicName);
    QTreeWidgetItem* add_TreeWidgetItem_MarkerArray(QTreeWidget* treeWidget, QString& displayName, QString& topicName);
    QTreeWidgetItem* add_TreeWidgetItem_Image(QTreeWidget* treeWidget, QString& displayName, QString& topicName);//添加图像图层
    QTreeWidgetItem* add_TreeWidgetItem_Pose(QTreeWidget* treeWidget, QString& displayName, QString& topicName);//添加Pose图层
    QTreeWidgetItem* add_TreeWidgetItem_BoundingBoxArray(QTreeWidget* treeWidget, QString& displayName, QString& topicName);//添加BoundingBoxArray图层


    /******************************************
    ** 修改图层相关属性方法
    *******************************************/
    void slot_modify_grid_attribute();
    void slot_modify_TF_attribute();
    void slot_modify_LaserScan_attribute();
    void slot_modify_PointCloud2_attribute();
    void slot_modify_RobotModel_attribute();
    void slot_modify_Map_attribute();
    void slot_modify_Path_attribute();
    void slot_modify_PointStamped_attribute();
    void slot_modify_Marker_attribute();
    void slot_modify_MarkerArray_attribute();
    void slot_modify_Image_attribute();//修改图像图层属性
    void slot_modify_Pose_attribute();//修改Pose图层属性
    void slot_modify_BoundingBoxArray_attribute();//修改BoundingBoxArray图层属性

    /******************************************
    ** 从UI界面移除待删除的图层相关控件方法
    *******************************************/
    void remove_TreeWidgetItem_Grid(QTreeWidget* treeWidget, QTreeWidgetItem* item);
    void remove_TreeWidgetItem_TF(QTreeWidget* treeWidget, QTreeWidgetItem* item);
    void remove_TreeWidgetItem_LaserScan(QTreeWidget* treeWidget, QTreeWidgetItem* item);
    void remove_TreeWidgetItem_PointCloud2(QTreeWidget* treeWidget, QTreeWidgetItem* item);
    void remove_TreeWidgetItem_RobotModel(QTreeWidget* treeWidget, QTreeWidgetItem* item);
    void remove_TreeWidgetItem_Map(QTreeWidget* treeWidget, QTreeWidgetItem* item);
    void remove_TreeWidgetItem_Path(QTreeWidget* treeWidget, QTreeWidgetItem* item);
    void remove_TreeWidgetItem_PointStamped(QTreeWidget* treeWidget, QTreeWidgetItem* item);
    void remove_TreeWidgetItem_Marker(QTreeWidget* treeWidget, QTreeWidgetItem* item);
    void remove_TreeWidgetItem_MarkerArray(QTreeWidget* treeWidget, QTreeWidgetItem* item);
    void remove_TreeWidgetItem_Image(QTreeWidget* treeWidget, QTreeWidgetItem* item);

    void removeChildren(QTreeWidgetItem* item);

    void get_common_rviz_Display();
    void ReadGNSS(bool *state);
public slots:
    //Global Options相关槽函数
    void slot_set_global_options();
    void slot_set_function_options(int index);

    //style改变槽函数
    void slot_onStyleComboBox_text_changed();

    //point size编辑结束槽函数
    void slot_onPointSizeLineEdit_edit_finished();

    //docktreeWidget相关槽函数
    void slot_display_layer_description();//点击后显示提示内容----------------------------------------------------------------
    void slot_enable_remove_button(QTreeWidgetItem* current);//删除item按钮

    void slot_update_image(QImage);
    void slot_sub_image();
private slots:
    void on_button_add_layer_clicked();//----------------------------------------------------------------------------------





    void on_work_pbt_GNSSConnetion_clicked();

    void on_work_pbt_loadPath_clicked();

    void on_work_pbt_AutoNavigation_clicked();

private:
    Ui::work_Obstacle *ui;

    //Global Options树控件
    QTreeWidgetItem* Function;
    QTreeWidgetItem* Global;
    QComboBox* FFCBox;                    //Fixed Frame ComboBox
    MyPalette* BPalette;                  //Background Palette
    QComboBox *imageItemBox = new QComboBox();//图像comboBox
    AddLayer* addLayer;
    qrviz* myrviz;
    QHash<QTreeWidgetItem*, rviz::Display*> m_LayerHash;
    QNode *rvizNode;

};

#endif // WORK_OBSTACLE_H
