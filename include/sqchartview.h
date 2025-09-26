#ifndef SQCHARTVIEW_H
#define SQCHARTVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QtCharts>
#include <QtCharts/QChartView>
#include <QWheelEvent>
#include <QPointF>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QGraphicsEllipseItem>
#include <QList>



QT_CHARTS_USE_NAMESPACE
class SQChartView : public QChartView
{
    Q_OBJECT
public:
    //SQChartView(QWidget *parent);
    explicit SQChartView(QWidget *parent = nullptr); // 构造函数声明
    //explicit SQChartView(QChart *chart, QWidget *parent = nullptr);
    void addCustomItem(QPointF dataPoint);  // 添加自定义 item 的方法
    void resetView();//重置
    void saveAsImage();// 保存视图为图片至本地
protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
private:
    //void createCrossAxes();
    bool dragging;
    QPoint dragStartPos;
};

#endif // SQCHARTVIEW_H
