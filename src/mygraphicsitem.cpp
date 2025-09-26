#include "mygraphicsitem.h"

myGraphicsItem::myGraphicsItem()
{
    // 从资源文件加载图片，假设图片名称为 ":/images/head.png"
    headPixmap = QPixmap(":/pic/tractor.svg");
    //svgRenderer.Qt::QSvgRenderer::load(":/pic/tractor.svg");
    svgRenderer.load(QStringLiteral(":/pic/tractor.svg"));

    lines.append(QLineF(QPointF(0,0), QPointF(0,0)));
}
/*
boundingRect函数定义了一个以(0,0)为中心，宽度为1024，高度为540，且考虑了画笔宽度的矩形。
*/
QRectF myGraphicsItem::boundingRect() const
{
    //不考虑画笔宽度：没有用到碰撞检测，无需考虑画笔宽度
    return QRectF(-350,-200,700,400);//定义一个1024x512大小的矩型框
//考虑画笔宽度
//    return QRectF(-512 - penWidth / 2, -30 - penWidth / 2,
//                  1024 + penWidth, 540 + penWidth);

}

void myGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // 使用 QPainter 绘制黑色矩形
    //function：让这个item的范围被黑色填充，显示scene范围(大小)
    // painter->setBrush(Qt::black); // 设置填充颜色为黑色
    // painter->drawRect(boundingRect()); // 使用 boundingRect() 绘制矩形

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::HighQualityAntialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    QPen pen1;
    pen1.setColor(QColor(3, 95, 24));
    pen1.setWidth(1);
    painter->setPen(pen1);
    pen1.setCapStyle(Qt::RoundCap); // 设置线条末端为圆角
    pen1.setJoinStyle(Qt::RoundJoin); // 设置线条连接处为圆角
    painter->translate(0, 0);


    // 绘制轨迹
    if (lines.size() > 0) {
        for (int i = 0; i < lines.size() ; i++) {
            painter->drawLine(lines[i]);
        }
    }

    // 绘制SVG图像
    if (svgRenderer.isValid()) {
        QSizeF scaledSize(5, 5); // 设置缩放大小为50x50像素
        QRectF targetRect(latestPoint.x() - scaledSize.width() / 2 , latestPoint.y() - scaledSize.height() / 2, scaledSize.width(), scaledSize.height());

        painter->save(); // 保存当前的绘制状态
        painter->translate(latestPoint); // 移动到最新点
        painter->rotate(180); // 旋转180度
        painter->translate(-latestPoint); // 移回原点
        svgRenderer.render(painter, targetRect); // 渲染SVG
        painter->restore(); // 恢复到保存的状态
        painter->resetTransform(); // 只在必要时恢复原始变换
    } else {
        qDebug() << "SVG renderer is not valid!";
    }
}

void myGraphicsItem::SetFinalPathXY(qreal endX, qreal endY)
{
    // 假设在类中维护了一个当前点作为线段的起点，默认情况下可以是(0, 0)或者上一次的终点
    //QPointF startPoint = lines.isEmpty() ? QPointF(0, 0) : lines.last().p2(); // 如果lines为空，则从原点开始，否则从上一线段的终点开始
    QPointF startPoint = lines.last().p2(); // 如果lines为空，则从原点开始，否则从上一线段的终点开始
    QPointF endPoint(endX, endY);
    lines.append(QLineF(startPoint, endPoint));
    latestPoint = endPoint;
    update(); // 触发重绘
}

void myGraphicsItem::ClearItem()
{
    // 清除所有线段
    lines.clear();
    lines.append(QLineF(QPointF(0,0), QPointF(0,0)));
    SetFinalPathXY(0.0, 0.0);
    update(); // 触发重绘
    lines.append(QLineF(QPointF(0,0), QPointF(0,0)));
    SetFinalPathXY(0.0, 0.0);

    // 清除所有线段
    lines.clear();
    lines.append(QLineF(QPointF(0,0), QPointF(0,0)));
    SetFinalPathXY(0.0, 0.0);
    update(); // 触发重绘
    lines.append(QLineF(QPointF(0,0), QPointF(0,0)));
    SetFinalPathXY(0.0, 0.0);
}


