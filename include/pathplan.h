#ifndef PATHPLAN_H
#define PATHPLAN_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include "mygraphicsitem.h"
#include <QFileDialog>
#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QDebug>
#include <QProcess>
#include <QDir>
#include <QString>
#include "globalsignal.h"
#include "ui_pathplan.h"
#include "qnode.hpp"
#include "SysSetting.hpp"
#include "coverpathplan.h"

using namespace ros_qt_demo;
namespace Ui {
class pathPlan;
}

class pathPlan : public QMainWindow
{
    Q_OBJECT

public:
    explicit pathPlan(
            QWidget *parent = nullptr,
            QNode *RosNode = nullptr,
            SQChart *pathData = nullptr
    );
    ~pathPlan();
    void moveToCenter();//窗口居中化显示
    //void setABLine_DotLine(QPointF PointA,QPointF  PointB);//可删
    //void setHalfCircle_DotLine(QList<QPointF> tmpQList);//可删
//    QString getFilePath();//点击按钮，打开文件系统可以选择一个文件输出到界面上
//    QString getFilePath(const QString &defaultDir);//点击按钮，打开文件系统可以选择一个文件输出到界面上(指定目录下)
    void modifySavePath(const QString &filePath, const QString &csvFilePath);//执行对launch文件内容，指定变量的修改    
    void modifyWayPointLoader(const QString &filePath, const QString &csvFilePath);//执行对launch文件内容，指定变量的修改

    //void read_Launch_WayPointLoader();//可删，移至qnode中
    //QList<QPointF> readCSV_XY(const QString &filePath);//可删，移至global中
    void readCsv(const QString &filePath);//读取csv中到point保存至文件中，保留
    //void getHistoryLine_DotLine(QList<QPointF> tmpQList);//读取csv文件然后在界面上显示出来，可删

    //QString getSaveCSVPath();//点击按钮，选中文件夹输入文件名，返回路径+文件名，可删
    //void PointSaveToCSV(QList<QPointF>& tmpPointF, QString& filename);//与上个函数一同使用，可删

    // 全覆盖路径规划
    void setPathPlanPlace(QPointF &a,QPointF &b,QPointF &c,QPointF &d);


    QList<QPointF> ABHalfCircleGeneratePoints(const QPointF &start, const QPointF &end, double interval);
    QList<QPointF> ABLineGeneratePoints(const QPointF &start, const QPointF &end, double interval);




    void savePath_Launch(bool *state);//开始记录/停止记录 (改完，未验证！)

    bool isClockwiseArc(const QPointF &A, const QPointF &B, double headingDeg);//判断圆的顺时针还是逆时针

    QList<QPointF> generate_CCPP_S_Path(const QList<QPointF> &points, double interval);
    void savePath_CCPP_S_2_local(QList<QPointF> points);
public slots:
    void slot_update_finalPath(float,float);//接受处理全局信号(X和Y的值)
private slots:
    void on_pbt_SetA_clicked();

    void on_pbt_SetB_clicked();

    void on_ptb_getFilePath_clicked();

    void on_ptb_modifyLaunch_clicked();

    void on_ptb_readPath0_clicked();


    void on_ptb_getFilePath2_clicked();

    void on_ptb_SavePath_clicked();

    void on_ptb_modifyLaunch_2_clicked();



    void on_ptb_getFilePath3_clicked();

    void on_pbt_getPathAB_clicked();

    void on_pbt_getPathHalfCircle_clicked();

    void on_pbt_SavePathAB_clicked();

    void on_pbt_savePathHalfCircle_clicked();

    void on_tbn_getCCPP_S_clicked();

    void on_pbt_pathplan_clear_clicked();

    void on_pushButton_10_clicked();

    void on_pbt_save2pic_clicked();

    void on_tbn_getCCPP_O_clicked();

private:
    Ui::pathPlan *ui;

    //myGraphicsItem *myItem;         //定义农机item对象
    //QGraphicsScene *mainScene;      //定义场景scene对象

    double currentPathX = 0.0;            //暂存当前的坐标X
    double currentPathY = 0.0;            //暂存当前的坐标Y

    QPointF currentPointA;                  //点击按钮A，保存当前农机坐标
    QPointF currentPointB;                  //点击按钮B，保存当前农机坐标
    QString csvFilePath;                      //选中要导入launch文件的csv文件

//    QProcess *Process_ReadLaunch;
//    QProcess *Process_savePathLaunch;//savePath开始记录/停止记录

    QNode *RosNode_;
    SQChart *ChartItem_;
    coverPathPlan *ccpp_;
};

#endif // PATHPLAN_H
