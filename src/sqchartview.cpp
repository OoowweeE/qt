#include "sqchartview.h"
#include <QPointF>

SQChartView::SQChartView(QWidget *parent):QChartView(parent),dragging(false)
{
    setRenderHint(QPainter::Antialiasing); // 抗锯齿
//    auto *chart = new QChart();
//    setChart(chart);
//    chart->setTitle("hhh");
    //createCrossAxes();
}



void SQChartView::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0) {
        chart()->zoom(1.05);  // 鼠标向前滚动，放大
    } else {
        chart()->zoom(0.95); // 鼠标向后滚动，缩小
    }
    event->accept();
}



void SQChartView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        dragStartPos = event->pos();
    }
    QChartView::mousePressEvent(event);
}

void SQChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging) {
        QPointF delta = event->pos() - dragStartPos;
        chart()->scroll(-delta.x(), delta.y());
        dragStartPos = event->pos();
    }
    QChartView::mouseMoveEvent(event);
}

void SQChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = false;
    }
    QChartView::mouseReleaseEvent(event);
}



void SQChartView::resizeEvent(QResizeEvent *event)
{
    QChartView::resizeEvent(event);
    // 在调整大小时更新坐标轴的位置
    auto chart = this->chart();
//    chart->axes(Qt::Horizontal).first()->setRange(-8, 8);
//    chart->axes(Qt::Vertical).first()->setRange(-8, 8);
}

//测试添加额外的item到view中，并可以跟随缩放移动
void SQChartView::addCustomItem(QPointF dataPoint)
{
    QPointF position = chart()->mapToPosition(dataPoint);
    QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem(position.x() - 5, position.y() - 5, 10, 10);
    ellipse->setBrush(QBrush(Qt::red));
    chart()->scene()->addItem(ellipse);
}
// 保存视图为图片至本地
void SQChartView::saveAsImage()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("PNG Files (*.png);;JPEG Files (*.jpg *.jpeg)"));
    if (!fileName.isEmpty()) {
        qDebug() << "Saving image to:" << fileName;
        // 创建一个 QPixmap 对象，大小与 SQChartView 相同
        QPixmap pixmap(size());

        // 填充背景颜色为透明
        pixmap.fill(Qt::transparent);

        // 使用 QPainter 将 SQChartView 的内容渲染到 QPixmap
        QPainter painter(&pixmap);
        render(&painter);

        // 保存 QPixmap 到指定文件
        if (pixmap.save(fileName)) {
            qDebug() << "Image saved to" << fileName;
        } else {
            qWarning() << "Failed to save image to" << fileName;
        }
    }

}





































//void SQChartView::createCrossAxes()
//{
//    // 创建坐标轴
//    QValueAxis *axisX = new QValueAxis();
//    QValueAxis *axisY = new QValueAxis();

//    // 设置轴的范围，确保中心在(0, 0)
//    axisX->setRange(-10, 10); // X轴范围
//    axisY->setRange(-10, 10); // Y轴范围

//    // 设置坐标轴标题
//    axisX->setTitleText("X Axis");
//    axisY->setTitleText("Y Axis");

//    // 添加坐标轴到图表中
//    chart()->addAxis(axisX, Qt::AlignBottom);
//    chart()->addAxis(axisY, Qt::AlignLeft);

//    // 创建中心线（可选）
//    QLineSeries *lineX = new QLineSeries();
//    lineX->append(-10, 0);
//    lineX->append(10, 0);
//    chart()->addSeries(lineX);
//    lineX->attachAxis(axisX);
//    lineX->attachAxis(axisY); // 如果你想要将线附加到Y轴

//    QLineSeries *lineY = new QLineSeries();
//    lineY->append(0, -10);
//    lineY->append(0, 10);
//    chart()->addSeries(lineY);
//    lineY->attachAxis(axisX); // 如果你想要将线附加到X轴
//    lineY->attachAxis(axisY);

//    // 设置系列的样式（例如线条颜色）
//    lineX->setPen(QPen(Qt::black, 2));
//    lineY->setPen(QPen(Qt::black, 2));
//}
