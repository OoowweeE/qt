#ifndef SQCHART_H
#define SQCHART_H

#include <QObject>
#include <QtCharts>
#include <QChart>
#include <QSplineSeries>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QPixmap>
#include <QPainter>
#include <QSvgRenderer>
#include <QMessageBox>
#include "rapidcsv.h"

QT_CHARTS_USE_NAMESPACE
//存储经纬度和高程
struct GpsCoordinates {
    double latitude;
    double longitude;
    double altitude;
};
//存储二维平面上的X和Y坐标
struct EnuCoordinates {
    double x;
    double y;
};
class SQChart : public QChart
{
    Q_OBJECT
public:
    SQChart();
    //设置轨迹颜色
    void setSplineSeriesPen(QLineSeries *series, Qt::PenStyle style, int width, QColor color);

    QChart *chart_navi_XY;//传入二维XY信息
    QChart *chart_pathPlan_XY;//传入二维信息，但用在路径规划页面中
    QChart *chart_GNSS;//传入经纬度坐标
    QChart *chart_harvest_XY;//传入二维信息，但是用在收获页面中

    // 导航页序列
    QSplineSeries *tractorRealPathSeries;//农机真实运动轨迹(XY)
    QSplineSeries *tractorRealPathSeries_GNSS;//农机真实运动轨迹（Lo、La）
    //QSplineSeries *tractorPlanPathSeries;//农机目标运动轨迹
    QLineSeries  *tractorPlanPathSeries;//农机目标运动轨迹
    QScatterSeries *tractorRealPathSeriesCar;//农机真实运动轨迹车头
    QScatterSeries *tractorRealPathSeriesCar_GNSS;//农机真实运动轨迹车头
    // 路径规划页序列
    QSplineSeries *tractorRealPathSeries_pathPlan;//农机真实运动轨迹(XY)
    //QSplineSeries *tractorPlanPathSeries_pathPlan;//农机目标运动轨迹
    QLineSeries *tractorPlanPathSeries_pathPlan;//农机目标运动轨迹
    QScatterSeries *tractorRealPathSeriesCar_pathPlan;//农机真实运动轨迹车头
    // 产量监测序列
    QSplineSeries *tractorRealPathSeries_harvest;//农机真实运动轨迹(XY)
    QScatterSeries *tractorRealPathSeriesCar_harvest;//农机真实运动轨迹车头

    QPointF lastPoint;//保存上一个点
    bool hasLastPoint = false;
    void addPointWithYieldColor(QPointF newPoint, double yield);
    void addPointWithYieldColor(float x,float y, double yield);

    // 添加坐标轴的封装函数
    void addSeriesWithAxes(QAbstractSeries *series);
    void Add_Series_Axis(QChart *chart,QAbstractSeries *series,QValueAxis *Axis_x,QValueAxis *Axis_y);

    void setRealPathPointF(QPointF *pointF);
    void SetPlanPathPointF(QPointF *pointF);


    void resetView();//重置平移与缩放的视图

    QPixmap  loadSvgAsPixmap(const QString &filePath,const QSize &size);//将svg转成pixmap
    void setTractorRealPathSeriesCar(double x,double y,double jiaodu);

    void drawPath(QChart* chart, const QPointF& p1, const QPointF& p2, const QPointF& p3, const QPointF& p4);

    void setABLine_DotLine(QPointF PointA, QPointF PointB);//画直线
    void setHalfCircleLine_DotLine(QPointF PointA, QPointF PointB, bool clockwise);//画曲线


    // 存放CSV中的点变量
    QList<QPointF> CsvPointF;

    // 将HistoryPointQList点集输出到Chart中显示
    void getHistoryPoint_DotLine(QList<QPointF> HistoryPointQList);


    //-----------------------------------------------------------------------------------------------------------------
    // 经纬坐标转换XY坐标函数：convertGpsToEnu
    void convertGpsToXY();
    double rad(double d);//角度制转弧度制
    // 实现存储经纬度和高程的变量
    GpsCoordinates Gps_CPP_Point1;
    GpsCoordinates Gps_CPP_Point2;
    GpsCoordinates Gps_CPP_Point3;
    GpsCoordinates Gps_CPP_Point4;
    // 实现存储二维平面上的X和Y坐标
    EnuCoordinates XY_CPP_Point1;
    EnuCoordinates XY_CPP_Point2;
    EnuCoordinates XY_CPP_Point3;
    EnuCoordinates XY_CPP_Point4;
    void Init_CPP_Point();//将上面声明的结构体变量在这个函数里初始化！
    EnuCoordinates convertGpsToXY(//传入经纬坐标处理后返回XY坐标
        double i_latitude, double i_longtitude, double m_latitude, double m_longtitude
    );
    void testPrint_convertGpsToXY(EnuCoordinates XY_CPP_Point,double a,double b);











//----------------------down archive--------------------------------------------------------------------------------
    // 生成一个带标签的点
    void CreatePoint(QPointF point,int flag = 0);
    void CreatePoint_chart_navi_XY(QPointF point);
    void CreatePoint_chart_pathPlan_XY(QPointF point);
    void CreatePoint(QChart *chart,QPointF point,int flag,QValueAxis *Axis_x,QValueAxis *Axis_y);//被调用


    // 清空轨迹：实际运动路径
    void clearRealPath();
    // 清空轨迹：规划运动路径
    void clearPlanPath();

    //点击按钮，选中文件夹输入文件名，返回路径+文件名
    QString getSaveCSVPath();
    //与上个函数一同使用，实现将tmpPointF保存至指定路径文件中
    void PointSaveToCSV(QList<QPointF>& tmpPointF, QString& filename);

    //生成目标轨迹线——AB线（只是生成变量点，如需在图上绘制，需要配合getHistoryPoint_DotLine函数一起使用）
    QList<QPointF> ABLineGeneratePoints(const QPointF &start, const QPointF &end, double interval);
    //生成目标轨迹线——半圆（只是生成变量点，如需在图上绘制，需要配合getHistoryPoint_DotLine函数一起使用）
    QList<QPointF> ABHalfCircleGeneratePoints(const QPointF &start, const QPointF &end, double interval);    
    QList<QPointF> ABHalfCircleGeneratePoints(const QPointF &start, const QPointF &end, double interval, double workWidth, double minTurningRadius);
    QList<QPointF> ABHalfCircleGeneratePoints(const QPointF &start, const QPointF &end, double interval, bool clockwise);
    QList<QPointF> generateAlternatingPath(const QList<QPointF> &points, double interval);
private:
    //配置Chart属性
    void createChart();
    // 设置自定义坐标轴
    void setCustomAxes();//里面用到了函数：addSeriesWithAxes
    // 自定义设置QSplineSeries的Pen属性的函数，带有默认值
    //void setSplineSeriesPen(QLineSeries *series, Qt::PenStyle style, int width, QColor color);

    QValueAxis *axisX;
    QValueAxis *axisY;
    QValueAxis *axisX_pathplan_xy;
    QValueAxis *axisY_pathplan_xy;
    QValueAxis *axisX_harvest_xy;
    QValueAxis *axisY_harvest_xy;
    QValueAxis *axisX_GNSS;
    QValueAxis *axisY_GNSS;

    bool convertGpsToXY_init_state;//gps转xy坐标初始化状态，0为未初始化，1为初始化
    static double EARTH_RADIUS;//地球半径



};

#endif // SQCHART_H
