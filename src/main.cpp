/**
 * @file /src/main.cpp
 *
 * @brief Qt based gui.
 *
 * @date November 2010
 **/
/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtGui>
#include <QApplication>
#include <QFile>
#include <QTextStream>

#include "login.h"
#include "menu.h"
#include "work.h"
#include "terminal.h"
#include "setting.h"
//#include "sqorm.h"
#include "pathplan.h"
#include <iostream>
#include "globalsignal.h"
#include "PoseData.hpp"
#include "MachineStatus.hpp"
#include "mainwindow.h"
#include "qnode.hpp"
#include "sqchart.h"
#include "database.hpp"
#include <QtVirtualKeyboard/QVirtualKeyboardInputContext>
#include "coverpathplan.h" // 测试全覆盖路径规划算法

#include <QApplication>
#include "apphelp.h"

void testInstance();
void test_CPPP_transform();
QString getStyleSetting(const QString &fileName);//导入样式表的函数
/*****************************************************************************
** Main
*****************************************************************************/
using namespace ros_qt_demo;
int main(int argc, char **argv) {

    //是否启用屏幕键盘
    //qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    /*********************
    ** Qt
    **********************/
    QApplication app(argc, argv);
//    AppHelp help1;
//    help1.show();
    Database& db = Database::instance("database_SQ.db");
    //db.dbInit("database_SQ.db"); // 创建对象到时候就已经初始化了

    //绑定退出信号槽机制
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    //导入样式表-0
    QString styleSheet1 = getStyleSetting(":/qss/style_1.qss");
    QString styleSheet2 = getStyleSetting(":/qss/qssStyle_QToolBox.qss");
    app.setStyleSheet(styleSheet1+styleSheet2);

//    work work1(argc,argv);
//    work1.show();

    // 测试单例模式各数据输出
    //testInstance();
    //test_CPPP_transform();

    // 测试全覆盖路径规划算法


//    QNode RosNode(argc,argv);
//    SQChart *pathData = new SQChart();
//    MainWindow m1(argc,argv,nullptr,&RosNode,pathData);       //显示主窗口
//    m1.show();
//-------------------------------------------------------------------------------------------------------------
    // 创建SplashScreen的loginPage，登录成功后才进入主窗口MainWindow
    login *loginPage = new login();
    if (loginPage->exec()==QDialog::Accepted)
    {
        QNode RosNode(argc,argv);
        SQChart *pathData = new SQChart();
        qDebug() << "0520-1";
        MainWindow m1(argc,argv,nullptr,&RosNode,pathData);       //显示主窗口
        qDebug() << "0520-2";
        m1.show();
        qDebug() << "0520-3";
        return app.exec();    //应用程序正常运行
    }
    else
        return  0;
//--------------------------------------------------------------------------------------------------------------
    //menu z(argc,argv);
    //z.show();

    //pathPlan p;
    //p.show();
    //Terminal t;
    //t.show();
    //Setting set1(argc,argv);
    //set1.show();

    //SqOrm orm1;
    //orm1.addSqlConnection();
    //orm1.insertData();
//    login login1;
//    login1.show();


    int result = app.exec();

    return result;
}

// 导入样式表
QString getStyleSetting(const QString &fileName){
    QFile file(fileName); // QSS 文件路径
    if (!file.exists()) { // 检查文件是否存在
        qWarning("Style file does not exist: %s", qPrintable(fileName));
        return QString(); // 返回空字符串以指示失败
    }

    if (!file.open(QFile::ReadOnly)) { // 检查文件是否成功打开
        qWarning("Could not open style file: %s", qPrintable(file.errorString()));
        return QString(); // 返回空字符串以指示失败
    }

    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll(); // 读取文件内容
    file.close(); // 关闭文件
    return stylesheet; // 返回样式表
}

// 测试单例模式
void testInstance(){
    PoseData::instance().updateGNSSData(1.0,2.0,3.0);
    qDebug() << PoseData::instance().getGpsData();

    MachineStatus::instance().updateSpeed(66);
    qDebug() << MachineStatus::instance().getMachineStatus();

}

// 测试全覆盖路径规划算法
void test_CPPP_transform(){
    // 测试全覆盖路径规划算法
    coverPathPlan *cpp = new coverPathPlan;
    QList<QPointF> original_points = {
        QPointF(3, 6),
        QPointF(18, 5),
        QPointF(19, 28),
        QPointF(4, 25)
    };
    auto transformed = cpp->transform(original_points);
    QList<QPointF> transformed_points = transformed.first;
    double theta = transformed.second;
    // 输出转换后的点和旋转角度
    qDebug() << "转换后的点:";
    for (const QPointF& point : transformed_points) {
        qDebug() << "(" << point.x() << "," << point.y() << ")";
    }
    qDebug() << "旋转角度 (弧度):" << theta;
    // 使用合并函数进行回转转换和平移
    QList<QPointF> final_points = cpp->back_transform(original_points, transformed_points, -theta);
    // 输出回转后的点
    qDebug() << "回转后的点:";
    for (const QPointF& point : final_points) {
        qDebug() << "(" << point.x() << "," << point.y() << ")";
    }
    // 测试全覆盖路径规划算法
    cpp->getCPP_S(original_points,1);
}
