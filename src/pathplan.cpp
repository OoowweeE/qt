#include "pathplan.h"

#include "globalsignal.h"
#include <cmath>
#include <QDesktopWidget>
using namespace globalValue;//全局变量

pathPlan::pathPlan(QWidget *parent,QNode *RosNode,SQChart *pathData) :
    QMainWindow(parent),
    ui(new Ui::pathPlan)
{
    ui->setupUi(this);
    RosNode_ = RosNode;             //初始化RosNode
    ChartItem_ = pathData;          //初始化pathData
    ccpp_ = new coverPathPlan();

    ui->pathPlanView->setChart(ChartItem_->chart_pathPlan_XY);
    ui->pathPlanView->setBackgroundBrush(Qt::white);//Qt:White

    //ui->menubar->hide();
    moveToCenter();//窗口居中显示

    //connect绑定setting发射的全局信号
    connect(RosNode_, &QNode::finalPathXY_vel,this, &pathPlan::slot_update_finalPath);
    connect(ui->pathplan_ptb_closeWindow,&QPushButton::clicked,this,&pathPlan::close);
}

pathPlan::~pathPlan()
{
    delete ui;
}

//将窗口居中显示
void pathPlan::moveToCenter()
{
    // 获取主屏幕的可用几何信息
    QDesktopWidget *desktop = QApplication::desktop();
    int screenWidth = desktop->width();//当前屏幕的x大小
    int screenHeight = desktop->height();//当前屏幕y大小

    // 计算弹窗的中心位置
    int x = (screenWidth - this->width()) / 2;
    int y = (screenHeight - this->height()) / 2;

    // 移动窗口(到屏幕中心)
    move(x, y);
}



//执行对launch文件内容，指定变量的修改
void pathPlan::modifyWayPointLoader(const QString &filePath, const QString &csvFilePath)
{
    QFile file(filePath); // 打开launch文件

    // 如果文件打开失败，输出错误提示
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for reading:" << file.errorString();
        return;
    }

    // 读取文件内容并解析为DOM文档
    QDomDocument document;
    if (!document.setContent(&file)) {
        qDebug() << "Failed to parse the file into a DOM tree.";
        file.close();
        return;
    }
    file.close(); // 解析完文件内容后可以关闭文件

    // 查找所有 <param> 标签
    QDomNodeList paramElements = document.elementsByTagName("param");
    for (int i = 0; i < paramElements.size(); ++i) {
        QDomElement paramElement = paramElements.at(i).toElement();

        // 判断 param 标签的 name 属性是否是 "path"
        if (paramElement.hasAttribute("name") && paramElement.attribute("name") == "path") {
            // 修改 value 属性，将新的 csv 文件路径设置为 value
            paramElement.setAttribute("value", csvFilePath);
            qDebug() << "Modified path to:" << csvFilePath;
            break; // 找到后就退出循环
        }
    }

    // 将修改后的内容写回文件
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for writing:" << file.errorString();
        return;
    }

    QTextStream out(&file);
    document.save(out, 4); // 将 DOM 文档内容保存回文件
    file.close();
}

void pathPlan::slot_update_finalPath(float x, float y)
{
    //myItem->SetFinalPathXY(x,y);
    currentPathX = x;
    currentPathY = -y;
}

void pathPlan::on_pbt_SetA_clicked()
{
    //currentPointB = QPointF(20,20);
    //ChartItem_->CreatePoint_chart_pathPlan_XY(currentPointB);
    // 设置标记项的位置
    currentPointA = QPointF(qreal(currentPathY), qreal(currentPathX));
    //---------------------------------------------------------------------------------------------------------
    //ChartItem_->CreatePoint(QPointF(qreal(currentPathY), qreal(currentPathX)),1);
    ChartItem_->CreatePoint_chart_pathPlan_XY(QPointF(qreal(currentPathY), qreal(currentPathX)));
    //---------------------------------------------------------------------------------------------------------
}


void pathPlan::on_pbt_SetB_clicked()
{
    // 将标记项的位置设置到场景坐标
    currentPointB = QPointF(qreal(currentPathY),qreal(currentPathX));
    //---------------------------------------------------------------------------------------------------------
    //ChartItem_->CreatePoint(QPointF(qreal(currentPathY), qreal(currentPathX)),2);
    ChartItem_->CreatePoint_chart_pathPlan_XY(QPointF(qreal(currentPathY), qreal(currentPathX)));
    //---------------------------------------------------------------------------------------------------------
}

//生成目标轨迹线——AB线
QList<QPointF> pathPlan::ABLineGeneratePoints(const QPointF &start, const QPointF &end, double interval)
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
//生成目标轨迹线——半圆
QList<QPointF> pathPlan::ABHalfCircleGeneratePoints(const QPointF &start, const QPointF &end, double interval) {
    QList<QPointF> points;
    // 计算圆心
    QPointF center((start.x() + end.x()) / 2.0, (start.y() + end.y()) / 2.0);
    // 计算半径
    double radius = std::sqrt(std::pow(end.x() - start.x(), 2) + std::pow(end.y() - start.y(), 2)) / 2.0;
    // 计算起始角和结束角
    double startAngle = std::atan2(start.y() - center.y(), start.x() - center.x());
    double endAngle = std::atan2(end.y() - center.y(), end.x() - center.x());
    // 确保起始角大于结束角（对于顺时针方向）
    if (startAngle < endAngle) {
        std::swap(startAngle, endAngle);
    }
    // 计算每一步的角度增量
    double dTheta = interval / radius;
    // 创建 XLSX/csv 文件

//    QXlsx::Document xlsx;
    //rapidcsv::Document doc("home/nvidia/")
    int row = 1; // XLSX 从第1行开始写入
    // 生成半圆上的点
    for (double theta = startAngle; theta >= endAngle; theta -= dTheta) {
        // 计算点的位置
        double x = center.x() + radius * std::cos(theta);
        double y = center.y() + radius * std::sin(theta);
        // 添加点到列表
        points.append(QPointF(x, y));
        // 写入点到 XLSX 文件
        ++row;
//        xlsx.write(row, 1, QString("%1").arg(x)); // 写入 x 坐标
//        xlsx.write(row, 2, QString("%1").arg(y)); // 写入 y 坐标
    }
    // 将最后一个 end 点写入 XLSX 文件
    ++row;
//    xlsx.write(row, 1, QString("%1").arg(end.x())); // 写入 x 坐标
//    xlsx.write(row, 2, QString("%1").arg(end.y())); // 写入 y 坐标
    // 保存 XLSX 文件到本地
//    xlsx.saveAs("Test_ABHalfCircle.csv");
    return points;
}

//点击按钮，获取filePath路径名
void pathPlan::on_ptb_getFilePath_clicked()
{
    // 1到目标目录下，选中预导入的目标csv文件
    QString defaultDirectory = "/home/nvidia/ws/src/waypoint_loader/waypoints";
    //csvFilePath = getFilePath(defaultDirectory);
    csvFilePath = QFileDialog::getOpenFileName(nullptr, "选择文件", defaultDirectory, "所有文件 (*)");
    ui->LineEdit_outputFilePath->setText(csvFilePath);// 将csv文件路径输出到界面中

    // 2 readCsv(csvFilePath);

    //read_Launch_WayPointLoader();
    // 2 RosNode_->getLoadPath(SysSetting::instance().loadPathLaunchState_);
    //1 getHistoryLine_DotLine(globalValue::readCsvPath);
}

//改指定launch文件里的内容（已写死，只针对waypoint_loader_updater）
void pathPlan::on_ptb_modifyLaunch_clicked()
{
    QString fileWayPointLoader = "/home/nvidia/ws/src/waypoint_updater/launch/waypoint_loader_updater.launch";

    modifyWayPointLoader(fileWayPointLoader,csvFilePath);

    // //状态取反，下一次关闭
    // flag_launch_WayPointLoader = !flag_launch_WayPointLoader;
}

//执行命令：roslaunch waypoint_updater waypoint_loader_updater.launch
//读取保存路径并更新最短跟随路径
void pathPlan::on_ptb_readPath0_clicked()
{

    //read_Launch_WayPointLoader();
    // 执行launch命令roslaunch waypoint_updater waypoint_loader_updater.launch
    RosNode_->getLoadPath(SysSetting::instance().loadPathLaunchState_);//执行launch文件
    // 获取csv文件的内容，保存为QPointF点ChartItem_->CsvPointF
    ChartItem_->CsvPointF = GlobalSignal::instance()->ReadCSV_XY(csvFilePath);
    // 将点输出到界面上
    ChartItem_->getHistoryPoint_DotLine(ChartItem_->CsvPointF);//将路径轨迹显示出来
}


void pathPlan::readCsv(const QString &filePath)
{
    ChartItem_->CsvPointF = GlobalSignal::instance()->ReadCSV_XY(filePath);
    //globalValue::readCsvPath = readCSV_XY(filePath);
}



void pathPlan::on_ptb_getFilePath2_clicked()
{
    //QString tmp_save_path_fileName= getSaveCSVPath();
    QString defaultDirectory = "/home/shine/Desktop";
    QString tmp_save_path_fileName=QFileDialog::getOpenFileName(nullptr, "选择文件", defaultDirectory, "所有文件 (*)");
    qDebug() << "文件将保存至：" <<tmp_save_path_fileName;//打印日志
    ui->LineEdit_inputFilePath->setText(tmp_save_path_fileName);//输出路径到界面中显示
    QString tmp_save_path_Launch = "/home/nvidia/ws/src/nmea_navsat_driver/launch/nmea_serial_driver_save_path.launch";
    modifySavePath(tmp_save_path_Launch,tmp_save_path_fileName);//第一个是launch文件地址，第二个是要改的csv文件地址
}


//执行对launch文件内容，指定变量的修改
void pathPlan::modifySavePath(const QString &filePath, const QString &csvFilePath)
{
    QFile file(filePath); // 打开launch文件

    // 如果文件打开失败，输出错误提示
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for reading:" << file.errorString();
        return;
    }

    // 读取文件内容并解析为DOM文档
    QDomDocument document;
    if (!document.setContent(&file)) {
        qDebug() << "Failed to parse the file into a DOM tree.";
        file.close();
        return;
    }
    file.close(); // 解析完文件内容后可以关闭文件

    // 查找所有 <param> 标签
    QDomNodeList paramElements = document.elementsByTagName("arg");
    for (int i = 0; i < paramElements.size(); ++i) {
        QDomElement paramElement = paramElements.at(i).toElement();

        // 判断 param 标签的 name 属性是否是 "path"
        if (paramElement.hasAttribute("name") && paramElement.attribute("name") == "save_path") {
            // 修改 value 属性，将新的 csv 文件路径设置为 value
            paramElement.setAttribute("default", csvFilePath);
            qDebug() << "Modified path to:" << csvFilePath;
            break; // 找到后就退出循环
        }
    }

    // 将修改后的内容写回文件
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for writing:" << file.errorString();
        return;
    }

    QTextStream out(&file);
    document.save(out, 4); // 将 DOM 文档内容保存回文件
    file.close();
}

// 已改完，已验证！
void pathPlan::savePath_Launch(bool *state)
{
    qDebug() <<"savePathLaunchState_: "<< QString::number(*state);
    //QString launchFile = getFilePath();
    if(!(*state)){//0对应开始记录，恢复控件显示为“停止记录”
        ui->ptb_SavePath->setText("停止记录");
    }else{                                             //1对应停止记录，恢复控件显示为“开始记录”
        ui->ptb_SavePath->setText("开始记录");
    }
    //调用savePath_Launch函数，根据当前的标志位savePathLaunchState_值进行执行是否启动launch文件
    RosNode_->savePath_Launch(*state);

}
void pathPlan::on_ptb_SavePath_clicked()
{
    savePath_Launch(&(SysSetting::instance().savePathLaunchState_)); //传入参数
}


void pathPlan::on_ptb_modifyLaunch_2_clicked()
{
    QString tmpfileName = ui->LineEdit_inputFilePath->text();
    QString tmpLaunch = "/home/nvidia/ws/src/nmea_navsat_driver/launch/nmea_serial_driver_save_path.launch";
    modifySavePath(tmpLaunch,tmpfileName);//第一个是launch文件地址，第二个是要改的csv文件地址
}



void pathPlan::on_ptb_getFilePath3_clicked()
{
    QString defaultDirectory = "/home/shine/Desktop";
    QString tmp_save_path_fileName=QFileDialog::getOpenFileName(nullptr, "选择文件", defaultDirectory, "所有文件 (*)");
    qDebug() << tmp_save_path_fileName;
    ui->lineEdit_autoPathPlan->setText(tmp_save_path_fileName);
}

void pathPlan::on_pbt_getPathAB_clicked()
{
    //在视图上生成AB直线
    //setABLine_DotLine(currentPointA,currentPointB);
    ChartItem_->setABLine_DotLine(currentPointA,currentPointB);
}

//1 = 顺时针，0=逆时针
bool pathPlan::isClockwiseArc(const QPointF& A,const QPointF& B,double headingDeg)
{
    double rad = qDegreesToRadians(headingDeg);//航向角（角度转弧度）
    QPointF headingVec(qCos(rad),qSin(rad));//航向单位向量
    QPointF ABVec = B-A;
    //计算叉积
    double cross = headingVec.x()*ABVec.y() - headingVec.y()*ABVec.x();
    return cross < 0;//小于0表示顺时针
}

void pathPlan::on_pbt_getPathHalfCircle_clicked()
{
    //在视图上生成半圆线setABLine_HalfCircle
    //ChartItem_->setHalfCircleLine_DotLine(ABHalfCircleGeneratePoints(currentPointA,currentPointB,0.1));
    ChartItem_->setHalfCircleLine_DotLine(currentPointA,currentPointB,1);
//    ChartItem_->setHalfCircleLine_DotLine(QPointF(0,0),QPointF(10,10),1);
//    ChartItem_->setHalfCircleLine_DotLine(QPointF(0,0),QPointF(10,-10),1);
//    ChartItem_->setHalfCircleLine_DotLine(QPointF(0,0),QPointF(-10,10),0);
//    ChartItem_->setHalfCircleLine_DotLine(QPointF(0,0),QPointF(-10,-10),0);
}

void pathPlan::setPathPlanPlace(QPointF &a, QPointF &b, QPointF &c, QPointF &d)
{

}

void pathPlan::on_pbt_SavePathAB_clicked()
{
    QList<QPointF> tmpQPointF = ChartItem_->ABLineGeneratePoints(currentPointA,currentPointB,0.2);
    QString tmpFileName = ui->lineEdit_autoPathPlan->text();
    ChartItem_->PointSaveToCSV(tmpQPointF,tmpFileName);
    ui->lineEdit_autoPathPlan->clear();
}

void pathPlan::on_pbt_savePathHalfCircle_clicked()
{
    QList<QPointF> tmpQPointF = ChartItem_->ABHalfCircleGeneratePoints(currentPointA,currentPointB,0.2,1);
    // 创建 XLSX/csv 文件,不使用标签行
    QString tmpFileName = ui->lineEdit_autoPathPlan->text();
    ChartItem_->PointSaveToCSV(tmpQPointF,tmpFileName);
    ui->lineEdit_autoPathPlan->clear();
}

void pathPlan::savePath_CCPP_S_2_local(QList<QPointF> points)
{
    QList<QPointF> tmpQPointF = points;
    QString tmpFileName = ui->lineEdit_autoPathPlan->text();
    //QString tmpFileName = "CCPP_S_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    ChartItem_->PointSaveToCSV(tmpQPointF,tmpFileName);
}


void pathPlan::on_tbn_getCCPP_S_clicked()
{
    QList<QPointF> points;
    // 提取UI中的边界点
    points.append(QPointF(ui->lineEidt_ccppA_x->text().toDouble(), ui->lineEidt_ccppA_y->text().toDouble()));
    points.append(QPointF(ui->lineEidt_ccppB_x->text().toDouble(), ui->lineEidt_ccppB_y->text().toDouble()));
    points.append(QPointF(ui->lineEidt_ccppC_x->text().toDouble(), ui->lineEidt_ccppC_y->text().toDouble()));
    points.append(QPointF(ui->lineEidt_ccppD_x->text().toDouble(), ui->lineEidt_ccppD_y->text().toDouble()));

    for (const QPointF& point : points) {
        qDebug() << "(" << point.x() << "," << point.y() << ")";
    }

    // 提取S型路径规划算法的路径点
    QList<QPointF> CCPP_S_Points = ccpp_->getCPP_S(points, 10);
    QList<QPointF> CCPP_S_Points_ = generate_CCPP_S_Path(CCPP_S_Points,0.2);

    // 保存点至本地
    savePath_CCPP_S_2_local(CCPP_S_Points_);
    //
    qDebug() << "==============================================";
    qDebug() << "==============================================";
    qDebug() << "==============================================";
    qDebug() << "==============================================";
    qDebug() << "==============================================";
    qDebug() << "==============================================";
    qDebug() << "==============================================";
    for (const QPointF& point : CCPP_S_Points_) {
        qDebug() << "(" << point.x() << "," << point.y() << ")";
    }

    // 在界面上画出来
    points.append(QPointF(ui->lineEidt_ccppA_x->text().toDouble(), ui->lineEidt_ccppA_y->text().toDouble()));
    ChartItem_->getHistoryPoint_DotLine(points);
    ChartItem_->getHistoryPoint_DotLine(CCPP_S_Points_);


}

QList<QPointF> pathPlan::generate_CCPP_S_Path(const QList<QPointF> &points, double interval)
{
    QList<QPointF> ccpp_s_path;

    // 检查是否有足够的点来生成路径
    if (points.size() < 2) {
        return ccpp_s_path;
    }

    // 添加从原点(0,0)到第一个目标点的初始路径
    QPointF start = QPointF(0, 0);
    QPointF end = points.first();
    ccpp_s_path.append(ChartItem_->ABLineGeneratePoints(start, end, interval));

    // 记录上一段路径的终点和航向角
    QPointF lastPoint = end;
    double lastHeading = 0.0;

    // 计算初始直线段的航向角
    double dx = end.x() - start.x();
    double dy = end.y() - start.y();
    lastHeading = qRadiansToDegrees(qAtan2(dy, dx));

    // 从第一个点开始，依次处理每对相邻的点
    for (int i = 0; i < points.size() - 1; ++i) {
        start = points[i];
        end = points[i + 1];

        // 根据当前段的索引确定使用直线还是半圆
        // 奇数段用半圆，偶数段用直线
        if (i % 2 == 1) {
            // 半圆路径需要基于当前线段的起点和终点计算参考方向
            dx = points[i].x() - points[i-1].x();
            dy = points[i].y() - points[i-1]

                    .y();
            double segmentHeading = qRadiansToDegrees(qAtan2(dy, dx));

            // 判断半圆方向
            bool clockwise = isClockwiseArc(start, end, segmentHeading);

            // 添加半圆路径（奇数段）
            ccpp_s_path.append(ChartItem_->ABHalfCircleGeneratePoints(start, end, interval, clockwise));

            // 更新lastHeading为半圆结束后的方向（与线段方向垂直）
            lastHeading = segmentHeading + (clockwise ? -90.0 : 90.0);
        } else {
            // 添加直线路径（偶数段）
            ccpp_s_path.append(ChartItem_->ABLineGeneratePoints(start, end, interval));

            // 更新lastHeading为当前直线段的方向
            dx = end.x() - start.x();
            dy = end.y() - start.y();
            lastHeading = qRadiansToDegrees(qAtan2(dy, dx));
        }

        // 更新lastPoint为当前段的终点
        lastPoint = end;
    }

    return ccpp_s_path;
}


//QList<QPointF> pathPlan::generate_CCPP_S_Path(const QList<QPointF> &points, double interval)
//{
//    QList<QPointF> ccpp_s_path;

//    // 检查是否有足够的点来生成路径
//    if (points.size() < 2) {
//        return ccpp_s_path;
//    }

//    // 添加从原点(0,0)到第一个目标点的初始路径
//    QPointF start = QPointF(0, 0);
//    QPointF end = points.first();
//    ccpp_s_path.append(ABLineGeneratePoints(start, end, interval));

//    // 从第一个点开始，依次处理每对相邻的点
//    for (int i = 0; i < points.size() - 1; ++i) {
//        start = points[i];
//        end = points[i + 1];

//        // 根据当前段的索引确定使用直线还是半圆
//        // 奇数段用半圆，偶数段用直线
//        if (i % 2 == 1) {
//            // 添加半圆路径（奇数段）
//            // 这里假设顺时针方向，您可以根据需要修改
//            ccpp_s_path.append(ChartItem_->ABHalfCircleGeneratePoints(start, end, interval, true));
//        } else {
//            // 添加直线路径（偶数段）
//            ccpp_s_path.append(ChartItem_->ABLineGeneratePoints(start, end, interval));
//        }
//    }

//    return ccpp_s_path;
//}


void pathPlan::on_pbt_pathplan_clear_clicked()
{
    ChartItem_->clearPlanPath();
}



void pathPlan::on_pushButton_10_clicked()
{
    ChartItem_->clearRealPath();
}

void pathPlan::on_pbt_save2pic_clicked()
{
    ui->pathPlanView->saveAsImage();
}

void pathPlan::on_tbn_getCCPP_O_clicked()
{
    QList<QPointF> points;
    // 提取UI中的边界点
    points.append(QPointF(ui->lineEidt_ccppA_x->text().toDouble(), ui->lineEidt_ccppA_y->text().toDouble()));
    points.append(QPointF(ui->lineEidt_ccppB_x->text().toDouble(), ui->lineEidt_ccppB_y->text().toDouble()));
    points.append(QPointF(ui->lineEidt_ccppC_x->text().toDouble(), ui->lineEidt_ccppC_y->text().toDouble()));
    points.append(QPointF(ui->lineEidt_ccppD_x->text().toDouble(), ui->lineEidt_ccppD_y->text().toDouble()));

    for (const QPointF& point : points) {
        qDebug() << "(" << point.x() << "," << point.y() << ")";
    }

    // 提取S型路径规划算法的路径点
    QList<QPointF> CCPP_O_Points = ccpp_->getCPP_O(points, 10);

    qDebug() << "==============================================";
    qDebug() << "==============================================";
    qDebug() << "==============================================";
    qDebug() << "==============================================";
    qDebug() << "==============================================";
    qDebug() << "==============================================";
    qDebug() << "==============================================";
    for (const QPointF& point : CCPP_O_Points) {
        qDebug() << "(" << point.x() << "," << point.y() << ")";
    }
    // 在界面上画出来
    points.append(QPointF(ui->lineEidt_ccppA_x->text().toDouble(), ui->lineEidt_ccppA_y->text().toDouble()));
    //ChartItem_->getHistoryPoint_DotLine(points);
    ChartItem_->getHistoryPoint_DotLine(CCPP_O_Points);
}
