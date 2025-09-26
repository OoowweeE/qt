#ifndef WORK_H
#define WORK_H
#include "ui_work.h"

#include "sqchartview.h"
#include "sqchart.h"
#include <QMainWindow>

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include "mygraphicsitem.h"
//#include "main_window.hpp"
#include <QDateTime>
#include <QtMath>
#include "setting.h"
#include <QScreen>
#include <QDesktopWidget>
//全局信号头文件globalsignal
#include "globalsignal.h"
#include <QProcess>
#include "pathplan.h"
#include <random>
#include <iostream>
#include <QPixmap>
#include <QPainter>
#include <QFileDialog>
#include <QStringList>

#include "harvest.hpp"
QT_CHARTS_USE_NAMESPACE
//命名空间！！！

#define workMode 1

namespace Ui {
class work;
}

class work : public QMainWindow
{
    Q_OBJECT

public:
    explicit work(int argc, char** argv,QWidget *parent = nullptr);
    ~work();

    void moveToCenter();
    void getConnectWithDevice();
    void getDisConnectWithDevice();
    void killRosbagProcesses();
    float getRandomFloat(float min,float max);
    QString getFilePath(const QString &defaultDir);//打开文件管理器，选中文件返回文件路径名
    QString getSaveCSVPath();//选择保存的文件路径

    void getGNSSTransform(bool state);//开启/关闭GNSS转换
    void getCanTransform(bool state);//开启/关闭产测转换
    void getCanDataFromRos(QString canString,canData_t &canData_tmp);
    void CanSaveToCSV(canData_t &canData_tmp, QString& filename);           //搭配getSaveCSVPath使用
    void InitCSV(QString& filename);
    QString CanSavePathString;//存放地址
    QString RosBagSavePathString;//rosbag存放地址
    void killAllRosLaunch();
    void getCanData2Qt(QString canString1);

    void getHistoryLine_DotLine(QList<QPointF> tmpQList);//路径读取在图上生成目标路径
    QList<QPointF> readCSV_XY(const QString &filePath);//读取csv文件，然后保存至变量中
    void modifyWayPointLoader(const QString &filePath, const QString &csvFilePath);//修改对launch文件内容，指定变量的修改
    void read_Launch_WayPointLoader();//执行路径读取的launch文件
    void work_clearTractorRealPath();//重置车头及角度、清空路径、重置状态信息栏
signals:
    void signal_RosConncet(bool state);
    void signal_Cat1Conncet(bool state);
public slots:
    void slot_update_GNSS(float,float,float);
    void slot_update_velPos(float,float);
    void slot_update_finalPath(float,float);
    void slot_update_basePath(float,float);
    void slot_update_velSpeed(float,float);
    void slot_update_yaw(float);
    void slot_update_roll(float);
    void slot_update_pitch(float);
    void slot_update_bds(int,int);
    void slot_update_lsd_CAN(QString);

    void saveChartAsImage();
private slots:
    void on_work_pbt_setting_clicked();

    void on_work_pbt_mapEnlarge_clicked();

    void on_work_pbt_mapReduce_clicked();

    void on_work_pbt_renew_clicked();

    void on_work_pbt_DeviceConnetion_clicked();

    void on_work_pbt_ABLine_clicked();

    void on_work_pbt_AutoNavigation_clicked();

    void on_work_pbt_pathFollow_clicked();

    void on_work_pbt_reNavigation_clicked();

    void on_work_pbt_CloseState_clicked();

    void on_work_pbt_XinHaoState_clicked(bool checked);

    void on_work_pbt_JieKouState_clicked(bool checked);

    void on_work_pbt_CanDeviceConnetion_clicked(bool checked);


    void on_work_pbt_rosKill_clicked();


    void on_work_pbt_recordRosbag_clicked(bool checked);


    void on_work_pbt_mapClear_clicked();

    void on_pbt_tmpSetPath_clicked(bool checked);


private:
    Ui::work *ui;
    bool DeviceConnetionState = 1;
    //MainWindow mainwindow1;
    Setting setting1;
    pathPlan pathplan1;
    //class1_ros_qt_demo::QNode *qnode1 = MainWindow::QNode;
    myGraphicsItem *myItem;
    QGraphicsScene *mainScene;
    QProcess *Process_1; //肖师兄的第一个要求按钮
    QProcess *Process_followPath;//执行roslaunch命令，开启路径跟随
    QProcess *Process_GNSSTransform;
    QProcess *Process_CanTransform;//can
    QGraphicsPathItem* basePathItem;
    QProcess *Process_ReadLaunch;

    SQChart *ChartItem;





};

#endif // WORK_H
