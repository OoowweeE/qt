#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include "setting.h"

#include "login.h"
#include "apphelp.h"
#include "work_obstacle.h"
#include "work_navi.hpp"
#include "work_harvest.h"
#include "pathplan.h"

#include <QWidget>
#include "qnode.hpp"
#include "sqchart.h"

//#include "settingswindow.h"
//#include "userwindow.h"
//#include "permissionwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
using namespace ros_qt_demo;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //MainWindow(QWidget *parent = nullptr);
    MainWindow(
            int argc=0,
            char** argv=nullptr,
            QWidget *parent = nullptr,
            QNode *RosNode = nullptr,
            SQChart *pathData = nullptr
    );
    ~MainWindow();

    void moveToCenter();// 窗口居中显示
    void ensurePageRender();// 确保页面渲染正常

private slots:
    void slot_update_dateTime(QDate date);
    void slot_updateSatellite(int num, int state);
//    void onButtonClicked(int id);  // ←←← 加这一行
private:
    Ui::MainWindow *ui;

    QButtonGroup btnGroup;
    QNode *rosnode;
    Setting *settings;
    login *loginPage;
    AppHelp *helpPage;
    work_Obstacle *obstaclePage;
    work_navi *naviPage;
    work_harvest *harvestPage;
    pathPlan *pathPlanPage;

    void updateDate();

//    UserWindow userWnd;
//    PermissionWindow permissionWnd;
//    SettingsWindow settingsWnd;
};
#endif // MAINWINDOW_H
