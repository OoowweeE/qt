
#ifndef SQVIEW_H
#define SQVIEW_H

#include <QGraphicsView>

#include <QObject>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QScrollBar>
class SQView : public QGraphicsView
{
    Q_OBJECT
public:
    SQView(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event) override ;
    void mouseMoveEvent(QMouseEvent *event) override ;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
private:
    QPoint lastPos; // 存储鼠标上次的位置
    bool dragging;
};

#endif // SQVIEW_H
