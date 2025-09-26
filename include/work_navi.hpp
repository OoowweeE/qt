#ifndef WORK_NAVI_H
#define WORK_NAVI_H

#include <QWidget>
#include "sqchart.h"
#include "sqchartview.h"
#include "qnode.hpp"
#include "globalsignal.h"
#include <cmath>
#include "SysSetting.hpp"

using namespace ros_qt_demo;
namespace Ui {
class work_navi;
}

class work_navi : public QWidget
{
    Q_OBJECT

public:
    explicit work_navi(
            QWidget *parent = nullptr,
            QNode *RosNode = nullptr,
            SQChart *pathData = nullptr
    );
    ~work_navi();

    // 更新chart图中的坐标轨迹
    void UpdateChart_XY_jiaodu(float x, float y, float jiaodu);

    // btn:杀死ros相关节点
    void killRosNode();
    // btn:清除行驶轨迹
    void ClearPath();
    // 放大ChartView视图
    void map_Zoom_In();
    // 缩小ChartView视图
    void map_Zoom_Out();
    // 重置ChartView视图
    void map_Reset();
    // 保存ui—>chartView为图片至本地
    void map_savePic2local();
    // GNSS读取按钮
    void ReadGNSS(bool *state);
private:
    Ui::work_navi *ui;

    SQChart *ChartItem_;//轨迹图
    QNode *RosNode_;

public slots:
    void UpdateNaviGNSS(float longitude,float latitude,float altitude);
    void UpdateNaviXY(float x,float y);
    void UpdateNavi_yaw(float yaw);
    void UpdateNavi_roll(float roll);
    void UpdateNavi_pitch(float pitch);
    void UpdateNavi_speed(float speedX,float speedY);
    void UpdateNavi_satellite(int num,int state);


private slots:

    void on_work_pbt_GNSSConnetion_clicked();
    void on_work_pbt_loadPath_clicked();
    void on_work_pbt_AutoNavigation_clicked();
};

#endif // WORK_NAVI_H
