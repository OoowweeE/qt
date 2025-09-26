#ifndef WORK_HARVEST_H
#define WORK_HARVEST_H

#include <QWidget>
#include "sqchart.h"
#include "sqchartview.h"
#include "qnode.hpp"
#include "globalsignal.h"
#include <cmath>
#include "SysSetting.hpp"
#include "YieldData.hpp"
#include "rapidcsv.h"

using namespace ros_qt_demo;
namespace Ui {
class work_harvest;
}

class work_harvest : public QWidget
{
    Q_OBJECT

public:
    explicit work_harvest(
            QWidget *parent = nullptr,
            QNode *RosNode = nullptr,
            SQChart *pathData = nullptr
    );
    ~work_harvest();


    void getCanString2Qt(QString canString);//将String类型到can数据解析至ui界面中


    void UpdateNaviXY(float x, float y);
private:
    Ui::work_harvest *ui;
    QNode *RosNode_;
    SQChart *ChartItem_;
    QString CanSavePathString;
    QString RosBagSavePathString;
    float can_actual_per_Acre_Yield;
    float harvest_yaw_;
public slots:
    void UpdateHarvestData_CAN(QString data);
    void UpdateNavi_yaw(float yaw);
private slots:
    void on_work_pbt_CanDeviceConnetion_clicked(bool checked);
    void on_work_pbt_recordRosbag_clicked(bool checked);
};

#endif // WORK_HARVEST_H
