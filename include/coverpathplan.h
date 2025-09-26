#ifndef COVERPATHPLAN_H
#define COVERPATHPLAN_H

#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QPointF>
#include <QList>
#include <QDebug>
#include <cmath>
#include <limits>



class coverPathPlan
{
public:
    coverPathPlan();
    // 定义一个结构来存储斜率和截距
    struct LineEquation {
        double slope;
        double intercept;
    };
    QList<QPointF> getCPP_S(QList<QPointF> points,double working_wide);
    QList<QPointF> getCPP_O(QList<QPointF> points, double working_wide);

    // 最终public应该只存放两个函数，一个用于生成S型路径，一个用于生成o型路径（这里的内容仅测试需要才放在这里）
    // 坐标转换
    std::pair<QList<QPointF>, double> transform(const QList<QPointF>& points);
    // 反坐标变换
    QList<QPointF> back_transform(const QList<QPointF>& points_origin, const QList<QPointF>& points_transform, double angle);
    // 最终public应该只存放两个函数，一个用于生成S型路径，一个用于生成o型路径（这里的内容仅测试需要才放在这里）


    QList<QPointF> f_intersection_points(const QList<coverPathPlan::LineEquation> &line_equations);
    QPointF calculate_intersection(const coverPathPlan::LineEquation &l1, const coverPathPlan::LineEquation &l2);
    bool isValidPoint(const QPointF &point);
private:


    // 计算两点之间的直线斜率和截距
    LineEquation calculate_slope_intercept(const QPointF& p1, const QPointF& p2) const;
    // 计算所有边的直线方程，存到QList中
    QList<LineEquation> calculate_line_equations(const QList<QPointF>& points) const;

    // 计算x坐标、y坐标
    double calculate_x_coordinate(double slope, double intercept, double known_y) const;
    double calculate_y_coordinate(double slope, double intercept, double known_x) const;

    // 计算多边形的质心
    QPointF calculate_centroid(const QList<QPointF>& points) const;
    // 生成内部小四边形的直线方程
    QList<LineEquation> small_quad(const QList<LineEquation>& lines, const QList<QPointF>& points, double with_co) const;
    // 扫描线算法
    QList<QPointF> move_horizontally(const QList<LineEquation>& line_equations, double with_co, double y_plass, double limit3, double limit4);
    // {A, B, C, D, E, F, G, H}->{A, B, D, C, E, F, H, G}
    QList<QPointF> swapPoints(QList<QPointF>& points);
};

#endif // COVERPATHPLAN_H
