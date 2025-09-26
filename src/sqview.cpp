
#include "sqview.h"
#include "QGraphicsItem"
#include <QDebug>
#include <QOpenGLWidget>
SQView::SQView(QWidget *parent):QGraphicsView(parent)
{

    //设置拖拽图标：鼠标拖拽没有变化
    setDragMode(QGraphicsView::NoDrag);
    //允许交互
    setInteractive(true);
    // 设置滚动条策略：隐藏滚动条但仍然可以拖动
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   //隐藏水平滚动条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);     //隐藏竖直滚动条

    setViewport(new QOpenGLWidget());
    setCacheMode(QGraphicsView::CacheBackground);
    setRenderHint(QPainter::Antialiasing, true);          //抗锯齿
    setRenderHint(QPainter::SmoothPixmapTransform, true); // 平滑的像素变换
    setRenderHint(QPainter::HighQualityAntialiasing, true); // 高质量抗锯齿

}

void SQView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        lastPos = event->pos();
        dragging = true;
        setCursor(Qt::ClosedHandCursor); // 设置拖动时的光标样式
    }
    qDebug() << "Mouse Press Event: " << lastPos;
    QGraphicsView::mousePressEvent(event);
}

void SQView::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging) {
        // 将事件的位置从视图坐标转换为场景坐标，并计算与上一个位置的增量
        QPointF delta = mapToScene(event->pos()) - mapToScene(lastPos);
        // 更新 lastPos 为当前鼠标位置
        lastPos = event->pos();
        // 获取当前场景矩形
        QRectF sceneRect = this->sceneRect();
        qDebug() << "sceneRect：" << sceneRect;
        // ? 计算新的场景矩形，将其移动 delta 距离
        sceneRect.translate(-delta.x(), -delta.y());
        // 设置新的场景矩形
        this->setSceneRect(sceneRect);
        // 获取视口中心点的场景坐标
        QPointF newCenter = mapToScene(viewport()->rect().center()) - delta;
        // 将视图中心移动到新的中心点
        centerOn(newCenter);
    }
    QGraphicsView::mouseMoveEvent(event);
}

void SQView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && dragging)
    {
        dragging = false;
        setCursor(Qt::ArrowCursor); // 恢复光标样式
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void SQView::wheelEvent(QWheelEvent *event)
{
    int wheelValue = event->angleDelta().y();
    double ratio = (double)wheelValue / (double)1200 + 1;
    scale(ratio, ratio);
}

void SQView::keyPressEvent(QKeyEvent *event)
{
    if (event->key()==Qt::Key_Left) //顺时针旋转45度
        rotate(45);
    QGraphicsView::keyPressEvent(event);
}
