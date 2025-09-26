#include "sqchart.h"
#include <QSvgRenderer>
#include <QPixmap>
#include <QPainter>
double SQChart::EARTH_RADIUS = 6378.137;//初始化这个static变量

SQChart::SQChart()
{
    axisX = new QValueAxis;
    axisY = new QValueAxis;
    axisX_pathplan_xy = new QValueAxis;
    axisY_pathplan_xy = new QValueAxis;
    axisX_harvest_xy = new QValueAxis;
    axisY_harvest_xy = new QValueAxis;
    axisX_GNSS = new QValueAxis;
    axisY_GNSS = new QValueAxis;

    //初始化相关变量
    tractorRealPathSeriesCar = new QScatterSeries();//车头，注意是QScatterSeries类型的
    tractorRealPathSeriesCar_GNSS = new QScatterSeries();//车头，注意是QScatterSeries类型的
    tractorRealPathSeriesCar_pathPlan = new QScatterSeries();;//车头,
    tractorRealPathSeriesCar_harvest = new QScatterSeries();;//车头,

    tractorRealPathSeries = new QSplineSeries();//二维坐标下，真实运动轨迹
    //tractorPlanPathSeries = new QSplineSeries();//路径规划轨迹
    tractorPlanPathSeries = new QLineSeries();//路径规划轨迹

    tractorRealPathSeries_pathPlan = new QSplineSeries();//农机真实运动轨迹(XY)
    //tractorPlanPathSeries_pathPlan = new QSplineSeries();//农机目标运动轨迹
    tractorPlanPathSeries_pathPlan = new QLineSeries();//农机目标运动轨迹

    tractorRealPathSeries_GNSS = new QSplineSeries();//三维GPS坐标下，真实运动轨迹

    tractorRealPathSeries_harvest = new QSplineSeries();//农机真实运动轨迹(XY)




    createChart();
    // 将序列添加至对应的Chart中
    Add_Series_Axis(chart_navi_XY,tractorRealPathSeries,axisX,axisY);
    Add_Series_Axis(chart_navi_XY,tractorPlanPathSeries,axisX,axisY);
    Add_Series_Axis(chart_navi_XY,tractorRealPathSeriesCar,axisX,axisY);

    Add_Series_Axis(chart_pathPlan_XY,tractorRealPathSeries_pathPlan,axisX_pathplan_xy,axisY_pathplan_xy);
    Add_Series_Axis(chart_pathPlan_XY,tractorPlanPathSeries_pathPlan,axisX_pathplan_xy,axisY_pathplan_xy);
    Add_Series_Axis(chart_pathPlan_XY,tractorRealPathSeriesCar_pathPlan,axisX_pathplan_xy,axisY_pathplan_xy);

    Add_Series_Axis(chart_harvest_XY,tractorRealPathSeries_harvest,axisX_harvest_xy,axisY_harvest_xy);
    //Add_Series_Axis(chart_harvest_XY,tractorPlanPathSeries_pathPlan,axisX_pathplan_xy,axisY_pathplan_xy);
    Add_Series_Axis(chart_harvest_XY,tractorRealPathSeriesCar_harvest,axisX_harvest_xy,axisY_harvest_xy);

    // 配置各序列的Pen属性
    setSplineSeriesPen(tractorRealPathSeries, Qt::SolidLine, 10, QColor(QRgb(0x008200))); // 设置为绿色
    setSplineSeriesPen(tractorRealPathSeries_pathPlan, Qt::SolidLine, 10, QColor(QRgb(0x008200))); // 设置为绿色
    setSplineSeriesPen(tractorRealPathSeries_GNSS, Qt::SolidLine, 10, QColor(QRgb(0x008200))); // 设置为绿色
    setSplineSeriesPen(tractorRealPathSeries_harvest, Qt::SolidLine, 20, QColor(Qt::transparent)); // 设置为绿色

    setSplineSeriesPen(tractorPlanPathSeries, Qt::DotLine, 2, Qt::blue); // 设置为蓝色，虚线
    setSplineSeriesPen(tractorPlanPathSeries_pathPlan, Qt::DotLine, 2, Qt::blue); // 设置为蓝色，虚线

    //setABLine_DotLine(QPointF(0,0),QPointF(10,10));
    //setABLine_HalfCircle(QPointF(0,0), QPointF(10,10), 0);

    // // 测试生成AB线
    //getHistoryPoint_DotLine(ABLineGeneratePoints(QPointF(-10,-1),QPointF(12,12),0.5));
    // 测试生成半圆
    //getHistoryPoint_DotLine(ABHalfCircleGeneratePoints(QPointF(-10,-1),QPointF(12,12),0.5));

    // 测试生成点
    //CreatePoint_chart_navi_XY(QPointF(6,6));
    //CreatePoint_chart_pathPlan_XY(QPointF(6,6));

    setTractorRealPathSeriesCar(0,0,90);

    //---------------------------------------------------------------------------------------------

    //Init_CPP_Point();//初始化全覆盖路径规划边界点
    //testPrint_convertGpsToXY(XY_CPP_Point1,40.021333,116.285094);
//    testPrint_convertGpsToXY(XY_CPP_Point2,39.9042,116.4084);
//    testPrint_convertGpsToXY(XY_CPP_Point3,39.904158,116.4074);
//    testPrint_convertGpsToXY(XY_CPP_Point4,39.904158,116.4084);

    //    XY_CPP_Point1={0.0,0.0};
//    XY_CPP_Point2={0.0,0.0};
//    XY_CPP_Point3={0.0,0.0};
//    XY_CPP_Point4={0.0,0.0};

}
void SQChart::resetView()
{
    // 1. 重置缩放
    chart_navi_XY->zoomReset();
}

QPixmap  SQChart::loadSvgAsPixmap(const QString &filePath, const QSize &size)
{
    QSvgRenderer svgRenderer(filePath);
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent); // 设置透明背景

    QPainter painter(&pixmap);
    svgRenderer.render(&painter);
    painter.end();

    return pixmap;
}




//设置车头位置和角度
void SQChart::setTractorRealPathSeriesCar(double x,double y,double jiaodu)
{
    //---------------------------------nvai
    tractorRealPathSeriesCar->clear();
    tractorRealPathSeriesCar->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    tractorRealPathSeriesCar->setMarkerSize(36.0);
    QImage CarPng(36,36,QImage::Format_ARGB32);
    CarPng.fill(Qt::transparent);
    //QPixmap image = QPixmap("://car.ico");
    QPixmap image = loadSvgAsPixmap(":/pic/tractor1.svg",QSize(36,36));
    if (image.isNull()) {
        qDebug() << "Image not found or cannot be loaded.";
        return; // 或者进行其他处理
    }
    QPainter painter(&CarPng);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(painter.pen().color());
    painter.setPen(QRgb(0xf6a625));//# f6a625
    //旋转方向
    painter.translate(CarPng.width() / 2, CarPng.height() / 2); // 移动到中心
    painter.rotate(jiaodu-90); // 旋转
    painter.translate(-image.width() / 2, -image.height() / 2); // 回到左上角

    painter.drawPixmap(0,0,image);
    tractorRealPathSeriesCar->setBrush(CarPng);
    tractorRealPathSeriesCar->setPen(QColor(Qt::transparent));
    tractorRealPathSeriesCar->append(x,y);
    //---------------------------------nvai

    //---------------------------------pathplan
    tractorRealPathSeriesCar_pathPlan->clear();
    tractorRealPathSeriesCar_pathPlan->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    tractorRealPathSeriesCar_pathPlan->setMarkerSize(36.0);
//    QImage CarPng(36,36,QImage::Format_ARGB32);
//    CarPng.fill(Qt::transparent);
//    //QPixmap image = QPixmap("://car.ico");
//    QPixmap image = loadSvgAsPixmap(":/pic/tractor1.svg",QSize(36,36));
//    if (image.isNull()) {
//        qDebug() << "Image not found or cannot be loaded.";
//        return; // 或者进行其他处理
//    }
//    QPainter painter(&CarPng);
//    painter.setRenderHint(QPainter::Antialiasing);
//    painter.setBrush(painter.pen().color());
//    painter.setPen(QRgb(0xf6a625));//# f6a625
//    //旋转方向
//    painter.translate(CarPng.width() / 2, CarPng.height() / 2); // 移动到中心
//    painter.rotate(jiaodu-90); // 旋转
//    painter.translate(-image.width() / 2, -image.height() / 2); // 回到左上角

//    painter.drawPixmap(0,0,image);
    tractorRealPathSeriesCar_pathPlan->setBrush(CarPng);
    tractorRealPathSeriesCar_pathPlan->setPen(QColor(Qt::transparent));
    tractorRealPathSeriesCar_pathPlan->append(x,y);
    //---------------------------------pathplan

    //---------------------------------GNSS
    tractorRealPathSeriesCar_GNSS->clear();
    tractorRealPathSeriesCar_GNSS->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    tractorRealPathSeriesCar_GNSS->setMarkerSize(36.0);

    tractorRealPathSeriesCar_GNSS->setBrush(CarPng);
    tractorRealPathSeriesCar_GNSS->setPen(QColor(Qt::transparent));
    tractorRealPathSeriesCar_GNSS->append(x,y);
    //---------------------------------GNSS

    //---------------------------------harvest
    tractorRealPathSeriesCar_harvest->clear();
    tractorRealPathSeriesCar_harvest->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    tractorRealPathSeriesCar_harvest->setMarkerSize(36.0);

    tractorRealPathSeriesCar_harvest->setBrush(CarPng);
    tractorRealPathSeriesCar_harvest->setPen(QColor(Qt::transparent));
    tractorRealPathSeriesCar_harvest->append(x,y);
    //---------------------------------GNSS


}

//绘制地块边界
void SQChart::drawPath(QChart *chart, const QPointF &p1, const QPointF &p2, const QPointF &p3, const QPointF &p4)
{
    // 创建 QLineSeries 绘制四边形的边
    QLineSeries* lineSeries = new QLineSeries();

    // 添加四条边
    lineSeries->append(p1);
    lineSeries->append(p2);
    lineSeries->append(p2);
    lineSeries->append(p3);
    lineSeries->append(p3);
    lineSeries->append(p4);
    lineSeries->append(p4);
    lineSeries->append(p1);  // 闭合四边形

    // 设置四边形边框的样式
    lineSeries->setName("Polygon");
    lineSeries->setPen(QPen(Qt::black, 2)); // 设定边框的颜色和宽度

    // 将线条系列添加到图表
    chart->addSeries(lineSeries);

    // 将坐标轴绑定到 QLineSeries 上
    lineSeries->attachAxis(axisX);
    lineSeries->attachAxis(axisY);

    // 创建 ScatterSeries 用于绘制四个点
    QScatterSeries* scatterSeries = new QScatterSeries();
    scatterSeries->setName("Points");
    scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    scatterSeries->setMarkerSize(10); // 设置点的大小

    // 向 scatterSeries 中添加四个点
    scatterSeries->append(p1);
    scatterSeries->append(p2);
    scatterSeries->append(p3);
    scatterSeries->append(p4);

    // 将 ScatterSeries 添加到 QChart 中
    chart->addSeries(scatterSeries);

    // 将散点图与坐标轴绑定
    scatterSeries->attachAxis(axisX);
    scatterSeries->attachAxis(axisY);

    setTractorRealPathSeriesCar(39.462735,115.835877,90);
}




//创建chart
void SQChart::createChart()
{
    //1 创建表格，并进行相关设置
    chart_navi_XY = new QChart();
    chart_pathPlan_XY = new QChart();
    chart_GNSS = new QChart();
    chart_harvest_XY = new QChart();
        //chart_navi_XY配置
    chart_navi_XY->setMargins(QMargins(0,0,0,0));//设置边距
    chart_navi_XY->setBackgroundRoundness(0);//去除背景圆角
    chart_navi_XY->setContentsMargins(0, 0, 0, 0);  // 设置内容边距
    chart_navi_XY->legend()->hide(); // 隐藏图例
        //chart_pathPlan_XY配置
    chart_pathPlan_XY->setMargins(QMargins(0,0,0,0));//设置边距
    chart_pathPlan_XY->setBackgroundRoundness(0);//去除背景圆角
    chart_pathPlan_XY->setContentsMargins(0, 0, 0, 0);  // 设置内容边距
    chart_pathPlan_XY->legend()->hide(); // 隐藏图例
        //chart_GNSS配置
    chart_GNSS->setMargins(QMargins(0,0,0,0));//设置边距
    chart_GNSS->setBackgroundRoundness(0);//去除背景圆角
    chart_GNSS->setContentsMargins(0, 0, 0, 0);  // 设置内容边距
    chart_GNSS->legend()->hide(); // 隐藏图例
        //chart_harvest_XY配置
    chart_harvest_XY->setMargins(QMargins(0,0,0,0));//设置边距
    chart_harvest_XY->setBackgroundRoundness(0);//去除背景圆角
    chart_harvest_XY->setContentsMargins(0, 0, 0, 0);  // 设置内容边距
    chart_harvest_XY->legend()->hide(); // 隐藏图例
    //1 创建表格，并进行相关设置

    //2 标定坐标原点
        // navi_work的原点
    QScatterSeries *series0 = new QScatterSeries();
    series0->setName("scatter1");
    series0->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series0->setMarkerSize(10.0);
    series0->append(0, 0);
        // pathplan的原点
    QScatterSeries *series1 = new QScatterSeries();
    series1->setName("scatter1");
    series1->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series1->setMarkerSize(10.0);
    series1->append(0, 0);
        // work_harvest的原点
    QScatterSeries *series2 = new QScatterSeries();
    series1->setName("scatter1");
    series1->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series1->setMarkerSize(10.0);
    series1->append(0, 0);
    //2 标定坐标原点

    //3 绑定数据与坐标轴:1将系列添加到图表中；2为系列绑定坐标轴
    setCustomAxes();
    //addSeriesWithAxes(series0);
    Add_Series_Axis(chart_navi_XY,series0,axisX,axisY);
    Add_Series_Axis(chart_pathPlan_XY,series1,axisX_pathplan_xy,axisY_pathplan_xy);
    Add_Series_Axis(chart_harvest_XY,series2,axisX_harvest_xy,axisY_harvest_xy);
    //3 绑定数据与坐标轴:1将系列添加到图表中；2为系列绑定坐标轴


    //4 设置序列的Pen格式---------------配置Pen
//    QPen pen1;
//    pen1.setStyle(Qt::SolidLine);
//    pen1.setWidth(10);
//    pen1.setColor(QRgb(0x008200));//#008200 79AD28
//    tractorRealPathSeries->setPen(pen1);
//    tractorRealPathSeries_GNSS->setPen(pen1);

//    QPen pen2;
//    pen2.setStyle(Qt::DotLine);
//    pen2.setWidth(5);
//    pen2.setColor(Qt::blue);
//    tractorPlanPathSeries->setPen(pen2);
}

// 自定义设置QSplineSeries的Pen属性的函数，带有默认值
void SQChart::setSplineSeriesPen(QLineSeries* series,
                        Qt::PenStyle style = Qt::SolidLine,  // 默认实线
                        int width = 2,                      // 默认宽度为2
                        QColor color = Qt::black)           // 默认颜色为黑色
{
    if (!series) return;  // 防止空指针访问

    // 创建一个QPen对象并设置样式、宽度和颜色
    QPen pen;
    pen.setStyle(style);           // 设置线条样式
    pen.setWidth(width);           // 设置线条宽度
    pen.setColor(color);           // 设置线条颜色

    // 将QPen应用到QSplineSeries
    series->setPen(pen);
}

void SQChart::addPointWithYieldColor(QPointF newPoint, double yield)
{
    if (!hasLastPoint) {
        lastPoint = newPoint;
        hasLastPoint = true;
        return;
    }

    // 新建一段线
    QLineSeries *segment = new QLineSeries();
    segment->append(lastPoint);
    segment->append(newPoint);
    Add_Series_Axis(chart_harvest_XY,segment,axisX_harvest_xy,axisY_harvest_xy);


    if(0<yield&&yield<250){
        setSplineSeriesPen(segment,Qt::SolidLine, 20, QColor(QColor(QRgb(0xFF0000))));
    }else if(250<=yield&&yield<500){
        setSplineSeriesPen(segment,Qt::SolidLine, 20, QColor(QColor(QRgb(0xFF6B6B))));
    }else if(500<=yield&&yield<750){
        setSplineSeriesPen(segment,Qt::SolidLine, 20, QColor(QColor(QRgb(0xFFA500))));
    }else if(750<=yield&&yield<1000){
        setSplineSeriesPen(segment,Qt::SolidLine, 20,QColor(QColor(QRgb(0xFFFF00))));
    }else if(1000<=yield&&yield<1250){
        setSplineSeriesPen(segment,Qt::SolidLine, 20, QColor(QColor(QRgb(0x90EE90))));
    }else if(yield>=1250){
        setSplineSeriesPen(segment,Qt::SolidLine, 20,QColor(QColor(QRgb(0x006400))));
    }else{
        setSplineSeriesPen(segment,Qt::SolidLine, 20, QColor(QColor(Qt::transparent)));
    }
    //setSplineSeriesPen(segment, Qt::SolidLine, 20, QColor(Qt::transparent)); // 设置为绿色

    lastPoint = newPoint;//更新上一个点
}

void SQChart::addPointWithYieldColor(float x, float y, double yield)
{

}

void SQChart::setABLine_DotLine(QPointF PointA, QPointF PointB)
{
    // 创建 QLineSeries 对象
    QLineSeries *lineSeries = new QLineSeries();

    // 添加起点和终点
    lineSeries->append(PointA);
    lineSeries->append(PointB);

    // 创建 QPen 设置线条为虚线
    QPen pen;
    pen.setStyle(Qt::DotLine); // 设置虚线样式
    pen.setWidth(5); // 设置线条宽度
    pen.setColor(Qt::darkBlue); // 设置线条颜色为蓝色
    // 应用画笔
    lineSeries->setPen(pen);

    // 添加对象
    //addSeriesWithAxes(lineSeries);
    Add_Series_Axis(chart_pathPlan_XY,lineSeries,axisX_pathplan_xy,axisY_pathplan_xy);
}

void SQChart::setHalfCircleLine_DotLine(QPointF PointA, QPointF PointB, bool clockwise)
{
    // 1. 计算圆心：中点
    QPointF center((PointA.x() + PointB.x()) / 2.0, (PointA.y() + PointB.y()) / 2.0);

    // 2. 半径
    qreal radius = QLineF(PointA, PointB).length() / 2.0;

    // 3. 计算起始和结束角
    qreal startAngle = std::atan2(PointA.y() - center.y(), PointA.x() - center.x());
    qreal endAngle = std::atan2(PointB.y() - center.y(), PointB.x() - center.x());

    // 4. 处理跨越 -π 到 π 的问题，统一到 [0, 2π)
    if (startAngle < 0) startAngle += 2 * M_PI;
    if (endAngle   < 0) endAngle   += 2 * M_PI;

    // 5. 顺时针调整角度（往负方向转）
    qreal dTheta = 0.01 / radius; // 步长角度
    QLineSeries *lineSeries = new QLineSeries();

    if (clockwise) {
        // 顺时针需从 startAngle 递减到 endAngle（跨 0 处理）
        if (startAngle < endAngle) startAngle += 2 * M_PI;
        for (qreal theta = startAngle; theta >= endAngle; theta -= dTheta) {
            qreal x = center.x() + radius * qCos(theta);
            qreal y = center.y() + radius * qSin(theta);
            lineSeries->append(x, y);
        }
    } else {
        // 逆时针从 startAngle 增加到 endAngle（跨 0 处理）
        if (endAngle < startAngle) endAngle += 2 * M_PI;
        for (qreal theta = startAngle; theta <= endAngle; theta += dTheta) {
            qreal x = center.x() + radius * qCos(theta);
            qreal y = center.y() + radius * qSin(theta);
            lineSeries->append(x, y);
        }
    }

    // 6. 设置样式
    QPen pen;
    pen.setColor(Qt::blue);
    pen.setWidth(2);
    lineSeries->setPen(pen);

    // 7. 添加进图表（你自己的函数）
    Add_Series_Axis(chart_pathPlan_XY, lineSeries, axisX_pathplan_xy, axisY_pathplan_xy);
}

//void SQChart::setHalfCircleLine_DotLine(QPointF PointA, QPointF PointB, bool clockwise)
//{
//    // 1. 计算圆心：两个点的中点
//    QPointF center((PointA.x() + PointB.x()) / 2, (PointA.y() + PointB.y()) / 2);

//    // 2. 计算半径：点A到点B的距离的一半
//    qreal radius = QLineF(PointA, PointB).length() / 2.0;

//    // 3. 计算起始角度和结束角度
//    qreal startAngle = std::atan2(PointA.y() - center.y(), PointA.x() - center.x());
//    qreal endAngle = std::atan2(PointB.y() - center.y(), PointB.x() - center.x());

//    // 4. 确保根据顺时针或逆时针调整角度范围
//    if (clockwise) {
//        // 顺时针时，如果startAngle小于endAngle，需要交换
//        if (startAngle < endAngle) {
//            std::swap(startAngle, endAngle);
//        }
//    } else {
//        // 逆时针时，如果startAngle大于endAngle，需要交换
//        if (startAngle > endAngle) {
//            std::swap(startAngle, endAngle);
//        }
//    }

//    // 5. 计算每步的角度增量
//    qreal dTheta = 0.01 / radius;  // 每一步的角度增量，0.01 是步长，可以根据需要调整

//    // 6. 创建一个 QLineSeries 来存储路径点
//    QLineSeries *lineSeries = new QLineSeries();

//    // 7. 生成圆弧上的点
//    if (clockwise) {
//        for (qreal theta = startAngle; theta >= endAngle; theta -= dTheta) {
//            qreal x = center.x() + radius * std::cos(theta);
//            qreal y = center.y() + radius * std::sin(theta);
//            lineSeries->append(x, y);  // 添加点到 QLineSeries 中
//        }
//    } else {
//        for (qreal theta = startAngle; theta <= endAngle; theta += dTheta) {
//            qreal x = center.x() + radius * std::cos(theta);
//            qreal y = center.y() + radius * std::sin(theta);
//            lineSeries->append(x, y);  // 添加点到 QLineSeries 中
//        }
//    }

//    // 8. 设置画笔
//    QPen pen;
//    pen.setColor(Qt::blue);  // 设置颜色为蓝色
//    pen.setWidth(2);         // 设置线条宽度
//    lineSeries->setPen(pen);

//    // 9. 将系列添加到图表中，并绑定坐标轴
//    //addSeriesWithAxes(lineSeries);
//    Add_Series_Axis(chart_pathPlan_XY,lineSeries,axisX_pathplan_xy,axisY_pathplan_xy);
//}




// 添加坐标轴的封装函数
void SQChart::addSeriesWithAxes(QAbstractSeries *series)
{
    // 将系列添加到图表中
    chart_navi_XY->addSeries(series);

    // 为系列绑定坐标轴
    series->attachAxis(axisX);
    series->attachAxis(axisY);
}

void SQChart::Add_Series_Axis(QChart *chart,QAbstractSeries *series, QValueAxis *Axis_x, QValueAxis *Axis_y)
{
    // 将系列添加到图表中
    chart->addSeries(series);

    // 为系列绑定坐标轴
    series->attachAxis(Axis_x);
    series->attachAxis(Axis_y);
}
// 设置自定义坐标轴
void SQChart::setCustomAxes()
{
    // 设置坐标轴范围
    axisX->setRange(-10, 10);  // X轴从 -20 到 20
    axisY->setRange(-10, 10);  // Y轴从 -10 到 10
    axisX_pathplan_xy->setRange(-10, 10);  // X轴从 -20 到 20
    axisY_pathplan_xy->setRange(-10, 10);  // Y轴从 -10 到 10
    axisX_harvest_xy->setRange(-10, 10);  // X轴从 -20 到 20
    axisY_harvest_xy->setRange(-10, 10);  // Y轴从 -10 到 10

    // 设置坐标轴的刻度数
    axisX->setTickCount(11);  // 刻度数为 5
    axisY->setTickCount(11);  // 刻度数为 5
    axisX_pathplan_xy->setTickCount(11);  // 刻度数为 5
    axisY_pathplan_xy->setTickCount(11);  // 刻度数为 5
    axisX_harvest_xy->setTickCount(11);  // 刻度数为 5
    axisY_harvest_xy->setTickCount(11);  // 刻度数为 5

    // 将坐标轴添加到图表中
    chart_navi_XY->addAxis(axisX, Qt::AlignBottom);  // 将 X 轴放在底部
    chart_navi_XY->addAxis(axisY, Qt::AlignLeft);    // 将 Y 轴放在左边
    chart_pathPlan_XY->addAxis(axisX_pathplan_xy, Qt::AlignBottom);  // 将 X 轴放在底部
    chart_pathPlan_XY->addAxis(axisY_pathplan_xy, Qt::AlignLeft);    // 将 Y 轴放在左边
    chart_harvest_XY->addAxis(axisX_harvest_xy, Qt::AlignBottom);  // 将 X 轴放在底部
    chart_harvest_XY->addAxis(axisY_harvest_xy, Qt::AlignLeft);    // 将 Y 轴放在左边

}

// 清空实际运动路径
void SQChart::clearRealPath()
{
    // 重置车头位置及角度
    this->setTractorRealPathSeriesCar(0,0,90);
    // 重置(清空)农机运动路径点
    this->tractorRealPathSeries->clear();

    this->tractorRealPathSeries_pathPlan->clear();

    this->tractorRealPathSeries_harvest->clear();
}

// 清空规划运动路径
void SQChart::clearPlanPath()
{
    // 重置车头位置及角度
    this->setTractorRealPathSeriesCar(0,0,90);
    // 重置(清空)农机运动路径点
    this->tractorPlanPathSeries->clear();
    this->tractorPlanPathSeries_pathPlan->clear();
}

void SQChart::getHistoryPoint_DotLine(QList<QPointF> HistoryPointQList)
{
    for (const QPointF &point : HistoryPointQList) {
        tractorPlanPathSeries->append(point);
        tractorPlanPathSeries_pathPlan->append(point);
    }
}

// 打开文件系统dialog，然后返回文件文件名
QString SQChart::getSaveCSVPath()
{
    // 打开文件保存对话框，选择保存的路径和文件名
    QString fileDIR = "/home/nvidia/ws/src/waypoint_loader/save_path";
    QString filePath = QFileDialog::getSaveFileName(nullptr, tr("保存文件"), fileDIR, tr("Text Files (*.csv);;All Files (*)"));

    // 检查用户是否选择了文件
    if (!filePath.isEmpty()) {
        // 弹出消息框，显示用户选择的文件名
        QMessageBox::information(nullptr, tr("文件已保存"), tr("文件保存到: %1").arg(filePath));
    }
    return filePath;// QString类型
}

void SQChart::PointSaveToCSV(QList<QPointF> &tmpPointF, QString &filePath)
{
    // 创建一个 rapidcsv::Document
    rapidcsv::Document doc("", rapidcsv::LabelParams(-1, -1));  // 不使用标签行

    // 遍历 QList<QPointF>，将 x 和 y 值存入 CSV 文件
    for (int i = 0; i < tmpPointF.size(); ++i) {
        // 获取 QPointF 的 x 和 y 值
        qreal x = tmpPointF[i].x();
        qreal y = tmpPointF[i].y();

        // 将 x 和 y 值分别存入到 CSV 的第一列和第二列
        doc.SetCell(0, i, x);  // 第一列存 x
        doc.SetCell(1, i, y);  // 第二列存 y
    }
    std::string savePath1 = filePath.toStdString();
    // 保存 CSV 文件
    doc.Save(savePath1);
}

void SQChart::CreatePoint(QPointF point,int flag)
{
    // 创建散点系列 (QScatterSeries)
    QScatterSeries *scatterSeries = new QScatterSeries();

    // 根据标识，设置散点样式，可以选择颜色、形状等
    scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    scatterSeries->setMarkerSize(20);
    if(flag == 0){
        scatterSeries->setColor(Qt::yellow); //黄色圆点
    }else if(flag == 1){
        QColor c1("#3a2462");
        scatterSeries->setColor(c1); //蓝色圆点
    }else if(flag == 2){
        QColor c2("#623a24");
        scatterSeries->setColor(c2); //紫色圆点
    }

    scatterSeries->append(point);
    // 添加点到系列中
    //chart_navi_XY->addSeries(scatterSeries);

    addSeriesWithAxes(scatterSeries);
}

void SQChart::CreatePoint_chart_navi_XY(QPointF point)
{
    CreatePoint(chart_navi_XY,point,1,axisX,axisY);
}

void SQChart::CreatePoint_chart_pathPlan_XY(QPointF point)
{
    CreatePoint(chart_pathPlan_XY,point,1,axisX_pathplan_xy,axisY_pathplan_xy);
}

void SQChart::CreatePoint(QChart *chart, QPointF point, int flag,QValueAxis *Axis_x,QValueAxis *Axis_y)
{
    // 创建散点系列 (QScatterSeries)
    QScatterSeries *scatterSeries = new QScatterSeries();

    // 根据标识，设置散点样式，可以选择颜色、形状等
    scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    scatterSeries->setMarkerSize(20);
    if(flag == 0){
        scatterSeries->setColor(Qt::yellow); //黄色圆点
    }else if(flag == 1){
        QColor c1("#3a2462");
        scatterSeries->setColor(c1); //蓝色圆点
    }else if(flag == 2){
        QColor c2("#623a24");
        scatterSeries->setColor(c2); //紫色圆点
    }

    // 添加点到系列中
    scatterSeries->append(point);
    Add_Series_Axis(chart,scatterSeries,Axis_x,Axis_y);
    //addSeriesWithAxes(scatterSeries);
}

//生成目标轨迹线——AB线
QList<QPointF> SQChart::ABLineGeneratePoints(const QPointF &start, const QPointF &end, double interval)
{
    QList<QPointF> points;
    double dx = end.x() - start.x();
    double dy = end.y() - start.y();
    double distance = std::sqrt(dx * dx + dy * dy);
    int numPoints = static_cast<int>(distance / interval);

    double stepX = dx / numPoints;
    double stepY = dy / numPoints;

    for (int i = 0; i <= numPoints; ++i) {
        double x = start.x() + i * stepX;
        double y = start.y() + i * stepY;
        points.append(QPointF(x, y));
    }
    return points;
}

//生成目标轨迹线——考虑转弯半径和作业幅宽的半圆/弓形
QList<QPointF> SQChart::ABHalfCircleGeneratePoints(const QPointF &start, const QPointF &end, double interval, double workWidth, double minTurningRadius) {
    QList<QPointF> points;
    
    // 计算圆心
    QPointF center((start.x() + end.x()) / 2.0, (start.y() + end.y()) / 2.0);
    double centerX = center.x();  // 缓存圆心x
    double centerY = center.y();  // 缓存圆心y
    
    // 计算半径
    double dx = end.x() - start.x();
    double dy = end.y() - start.y();
    double radius = std::sqrt(dx * dx + dy * dy) / 2.0;
    
    // 计算起始角和结束角
    double startAngle = std::atan2(start.y() - centerY, start.x() - centerX);
    double endAngle = std::atan2(end.y() - centerY, end.x() - centerX);
    
    // 确保起始角大于结束角（对于顺时针方向）
    if (startAngle < endAngle) {
        std::swap(startAngle, endAngle);
    }
    
    // 判断作业幅宽和最小转弯半径的关系
    if (workWidth == 2 * minTurningRadius) {
        // 作业幅宽等于最小转弯直径，使用半圆形转弯
        double dTheta = interval / radius;
        for (double theta = startAngle; theta >= endAngle; theta -= dTheta) {
            double x = centerX + radius * std::cos(theta);
            double y = centerY + radius * std::sin(theta);
            points.append(QPointF(x, y));
        }
    } else if (workWidth > 2 * minTurningRadius) {
        // 作业幅宽大于最小转弯直径，进行1/4圆转弯 -> 直行一段 -> 1/4圆转弯
        double quarterCircleAngle = M_PI / 2.0;  // 1/4圆的角度
        double dTheta = interval / radius;
        
        // 第1段 1/4圆
        for (double theta = startAngle; theta >= startAngle - quarterCircleAngle; theta -= dTheta) {
            double x = centerX + radius * std::cos(theta);
            double y = centerY + radius * std::sin(theta);
            points.append(QPointF(x, y));
        }
        
        // 直行部分（增加一定长度，假设直行长度为作业幅宽与最小转弯半径的差）
        double straightLength = workWidth - 2 * minTurningRadius;
        double straightX = points.last().x() + straightLength;
        double straightY = points.last().y();
        points.append(QPointF(straightX, straightY));
        
        // 第2段 1/4圆
        center = QPointF(straightX, straightY);  // 更新圆心位置
        for (double theta = 0; theta <= quarterCircleAngle; theta += dTheta) {
            double x = center.x() + radius * std::cos(theta);
            double y = center.y() + radius * std::sin(theta);
            points.append(QPointF(x, y));
        }
    }
    
    return points;
}


//生成目标轨迹线——半圆
QList<QPointF> SQChart::ABHalfCircleGeneratePoints(const QPointF &start, const QPointF &end, double interval, bool clockwise) {
    QList<QPointF> points;

    // 1. 计算圆心（AB中点）
    QPointF center((start.x() + end.x()) / 2.0, (start.y() + end.y()) / 2.0);

    // 2. 半径
    double radius = QLineF(start, end).length() / 2.0;

    // 3. 起始角、结束角
    double startAngle = std::atan2(start.y() - center.y(), start.x() - center.x());
    double endAngle = std::atan2(end.y() - center.y(), end.x() - center.x());

    // 4. 调整角度范围以确保是半圆（逆时针时角度必须增长，顺时针时角度必须减小）
    if (clockwise) {
        if (startAngle < endAngle) {
            startAngle += 2 * M_PI;  // 保证从 startAngle 减到 endAngle 是顺时针
        }
    } else {
        if (endAngle < startAngle) {
            endAngle += 2 * M_PI;  // 保证从 startAngle 增到 endAngle 是逆时针
        }
    }

    // 5. 角度步长（r = s/θ → θ = s/r）
    double dTheta = interval / radius;

    // 6. 生成点
    if (clockwise) {
        for (double theta = startAngle; theta >= endAngle; theta -= dTheta) {
            double x = center.x() + radius * std::cos(theta);
            double y = center.y() + radius * std::sin(theta);
            points.append(QPointF(x, y));
        }
    } else {
        for (double theta = startAngle; theta <= endAngle; theta += dTheta) {
            double x = center.x() + radius * std::cos(theta);
            double y = center.y() + radius * std::sin(theta);
            points.append(QPointF(x, y));
        }
    }

    // 7. 确保终点也加入
    points.append(end);

    return points;
}

//QList<QPointF> SQChart::ABHalfCircleGeneratePoints(const QPointF &start, const QPointF &end, double interval) {
//    QList<QPointF> points;
//    // 计算圆心
//    QPointF center((start.x() + end.x()) / 2.0, (start.y() + end.y()) / 2.0);
//    // 计算半径
//    double radius = std::sqrt(std::pow(end.x() - start.x(), 2) + std::pow(end.y() - start.y(), 2)) / 2.0;
//    // 计算起始角和结束角
//    double startAngle = std::atan2(start.y() - center.y(), start.x() - center.x());
//    double endAngle = std::atan2(end.y() - center.y(), end.x() - center.x());
//    // 确保起始角大于结束角（对于顺时针方向）
//    if (startAngle < endAngle) {
//        std::swap(startAngle, endAngle);
//    }
//    // 计算每一步的角度增量
//    double dTheta = interval / radius;
//    // 创建 XLSX/csv 文件

////    QXlsx::Document xlsx;
//    //rapidcsv::Document doc("home/nvidia/")
//    int row = 1; // XLSX 从第1行开始写入
//    // 生成半圆上的点
//    for (double theta = startAngle; theta >= endAngle; theta -= dTheta) {
//        // 计算点的位置
//        double x = center.x() + radius * std::cos(theta);
//        double y = center.y() + radius * std::sin(theta);
//        // 添加点到列表
//        points.append(QPointF(x, y));
//        // 写入点到 XLSX 文件
//        ++row;
////        xlsx.write(row, 1, QString("%1").arg(x)); // 写入 x 坐标
////        xlsx.write(row, 2, QString("%1").arg(y)); // 写入 y 坐标
//    }
//    // 将最后一个 end 点写入 XLSX 文件
//    ++row;
////    xlsx.write(row, 1, QString("%1").arg(end.x())); // 写入 x 坐标
////    xlsx.write(row, 2, QString("%1").arg(end.y())); // 写入 y 坐标
//    // 保存 XLSX 文件到本地
////    xlsx.saveAs("Test_ABHalfCircle.csv");
//    return points;
//}

void SQChart::convertGpsToXY()
{
    // 初始化判断
    if(!convertGpsToXY_init_state){// convertGpsToXY_init_state = 0 ：未初始化

        convertGpsToXY_init_state = true;
    }
}

double SQChart::rad(double d)
{
    return d * 3.1415926 / 180.0;
}

void SQChart::Init_CPP_Point()
{
    // 实现存储经纬度和高程的变量
    Gps_CPP_Point1={0.0,0.0,0.0};
    Gps_CPP_Point2={0.0,0.0,0.0};
    Gps_CPP_Point3={0.0,0.0,0.0};
    Gps_CPP_Point4={0.0,0.0,0.0};
    // 实现存储二维平面上的X和Y坐标
    XY_CPP_Point1={0.0,0.0};
    XY_CPP_Point2={0.0,0.0};
    XY_CPP_Point3={0.0,0.0};
    XY_CPP_Point4={0.0,0.0};
}

EnuCoordinates SQChart::convertGpsToXY(double i_latitude, double i_longtitude, double m_latitude, double m_longtitude)
{
    // 初始化判断
    if (!convertGpsToXY_init_state) {  // convertGpsToXY_init_state = 0 ：未初始化
        convertGpsToXY_init_state = true;
    }

    // 计算相对位置
    double radLat1, radLat2, radLong1, radLong2, delta_lat, delta_long, x, y;
    radLat1 = rad(i_latitude);
    radLong1 = rad(i_longtitude);
    radLat2 = rad(m_latitude);
    radLong2 = rad(m_longtitude);

    // 使用QString::number来控制精度
    qDebug() << "Input Latitude 1:" << QString::number(i_latitude, 'f', 6) << "Longitude 1:" << QString::number(i_longtitude, 'f', 6);
    qDebug() << "Input Latitude 2:" << QString::number(m_latitude, 'f', 6) << "Longitude 2:" << QString::number(m_longtitude, 'f', 6);

    // 计算X
    delta_long = 0;
    delta_lat = radLat2 - radLat1;  // (radLat1, radLong1) - (radLat2, radLong1)
    qDebug() << "delta_lat:" << QString::number(delta_lat, 'f', 6);

    if (delta_lat > 0) {
        x = 2 * asin(sqrt(pow(sin(delta_lat / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(delta_long / 2), 2)));
    } else {
        x = -2 * asin(sqrt(pow(sin(delta_lat / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(delta_long / 2), 2)));
    }
    x = x * EARTH_RADIUS * 1000;

    qDebug() << "Calculated X:" << QString::number(x, 'f', 6);

    // 计算Y
    delta_lat = 0;
    delta_long = radLong2 - radLong1;  // (radLat1, radLong1) - (radLat1, radLong2)
    qDebug() << "delta_long:" << QString::number(delta_long, 'f', 6);

    if (delta_long > 0) {
        y = 2 * asin(sqrt(pow(sin(delta_lat / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(delta_long / 2), 2)));
    } else {
        y = -2 * asin(sqrt(pow(sin(delta_lat / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(delta_long / 2), 2)));
    }
    y = y * EARTH_RADIUS * 1000;

    qDebug() << "Calculated Y:" << QString::number(y, 'f', 6);

    // 汇总数据
    EnuCoordinates XY_CPP_Point;
    XY_CPP_Point.x = x;
    XY_CPP_Point.y = y;

    // 输出最终结果
    qDebug() << "Final EnuCoordinates: X =" << QString::number(XY_CPP_Point.x, 'f', 6) << "Y =" << QString::number(XY_CPP_Point.y, 'f', 6);

    return XY_CPP_Point;
}


void SQChart::testPrint_convertGpsToXY(EnuCoordinates XY_CPP_Point,double a,double b)
{
    XY_CPP_Point = convertGpsToXY(40.027906,116.281503,a,b);
//    XY_CPP_Point2 = convertGpsToXY(39.9042,116.4074,39.915,116.4);
//    XY_CPP_Point3 = convertGpsToXY(39.9042,116.4074,39.915,116.4);
//    XY_CPP_Point4 = convertGpsToXY(39.9042,116.4074,39.915,116.4);
    qDebug() << "原点X：0.0，原点Y:0.0";
    qDebug() << "边界点X：" << QString::number(XY_CPP_Point.x,'f',10)
             << "边界点Y：" << QString::number(XY_CPP_Point.y,'f',10);
}

