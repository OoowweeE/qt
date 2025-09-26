#ifndef MYGRAPHICSITEM_H
#define MYGRAPHICSITEM_H
#include <QGraphicsItem>
#include <QPainter>
#include <QSvgRenderer>
#include <QDebug>

#include <QLineF>
#include <QList>

class myGraphicsItem : public QGraphicsItem
{
public:
    myGraphicsItem();
    //任何一个QGraphicsItem都需要重写boundingRect()和paint()函数
    QRectF boundingRect() const override;//重写boundingRect函数
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;//重写paint函数
    void SetFinalPathXY(qreal endX, qreal endY);//绘制路径函数
    void ClearItem();
private:
    QList<QLineF> lines;
    QPixmap headPixmap;
    QPointF latestPoint;
    QSvgRenderer svgRenderer;
};

#endif // MYGRAPHICSITEM_H
