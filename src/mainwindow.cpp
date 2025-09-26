#include "mainwindow.h"
#include "ui_mainwindow.h"


// 看这个构造函数
MainWindow::MainWindow(int argc, char **argv, QWidget *parent, QNode *RosNode,SQChart *pathData)
    : QMainWindow(parent)
    ,ui(new Ui::MainWindow)
//    ,settings(argc,argv,this)
//    ,loginPage(this)
//    ,helpPage(this)
//    ,obstaclePage(argc,argv,this,RosNode)
//    ,naviPage(this,RosNode,pathData)
//    ,harvestPage(this,RosNode,pathData)
//    ,pathPlanPage(this,RosNode,pathData)
{

    qDebug() << "0520-1-1";
    settings = new Setting(argc, argv, parent,RosNode);
    qDebug() << "0520-1-2";
    loginPage = new login(parent);
    qDebug() << "0520-1-3";
    helpPage = new AppHelp(parent);
    qDebug() << "0520-1-4";
    obstaclePage = new work_Obstacle(argc, argv, parent, RosNode);
    qDebug() << "0520-1-5";
    naviPage = new work_navi(parent, RosNode, pathData);
    qDebug() << "0520-1-6";
    harvestPage = new work_harvest(parent, RosNode, pathData);
    qDebug() << "0520-1-7";
    pathPlanPage = new pathPlan(parent, RosNode, pathData);


    ui->setupUi(this);
    rosnode = RosNode;

    this->setWindowTitle("智能农机车载终端");
    updateDate();//更新状态栏的日期
    moveToCenter();//窗口居中显示

    qDebug() << "btn_Navi: " << ui->btn_Navi;
    qDebug() << "btn_Obstacle: " << ui->btn_Obstacle;
    qDebug() << "btn_Havrest: " << ui->btn_Havrest;
    qDebug() << "btn_Pathplan: " << ui->btn_Pathplan;
    qDebug() << "btn_Setting: " << ui->btn_Setting;
    qDebug() << "btn_Help: " << ui->btn_Help;

    qDebug() << "0520-1-7-1";
    ui->stackedWidget->addWidget(naviPage);

    ui->stackedWidget->addWidget(obstaclePage);//防报错先隐藏
    ui->stackedWidget->addWidget(harvestPage);
    ui->stackedWidget->addWidget(pathPlanPage->centralWidget());

    ui->stackedWidget->addWidget(settings);//防报错先隐藏
    ui->stackedWidget->addWidget(helpPage);

//    ui->stackedWidget->addWidget(&settingsWnd);

//    Q_ASSERT(ui->btn_Navi != nullptr);
//    Q_ASSERT(ui->btn_Obstacle != nullptr);
//    Q_ASSERT(ui->btn_Havrest != nullptr);
//    Q_ASSERT(ui->btn_Pathplan != nullptr);
//    Q_ASSERT(ui->btn_Setting != nullptr);
//    Q_ASSERT(ui->btn_Help != nullptr);

    qDebug() << "0520-1-7-2";
//    btnGroup.setExclusive(false);
    btnGroup.addButton(ui->btn_Navi, 0);
    btnGroup.addButton(ui->btn_Obstacle, 1);
    btnGroup.addButton(ui->btn_Havrest, 2);
    btnGroup.addButton(ui->btn_Pathplan, 3);
    btnGroup.addButton(ui->btn_Setting, 4);
    btnGroup.addButton(ui->btn_Help, 5);

    qDebug() << "0520-1-7-3";
    // 绑定按键和stackedWidget序号
    connect(&btnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), ui->stackedWidget, &QStackedWidget::setCurrentIndex);
    // 每次页面切换时调用刷新函数
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, &MainWindow::ensurePageRender);

    // 全局更新QDate
    connect(settings,&Setting::signal_dateTimeChange,this,&MainWindow::slot_update_dateTime);

    connect(rosnode,&QNode::satellite_vel,this,&MainWindow::slot_updateSatellite);

    qDebug() << "0520-1-7-4";
    // 设置默认选中的页面
    btnGroup.button(0)->setChecked(true);
    ui->stackedWidget->setCurrentIndex(4);
    ui->btn_Setting->setChecked(1);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete settings;
    delete loginPage;
    delete helpPage;
    delete obstaclePage;
    delete naviPage;
    delete harvestPage;
    delete pathPlanPage;
}

//将窗口居中显示
void MainWindow::moveToCenter()
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

// 确保页面渲染正常
void MainWindow::ensurePageRender()
{
    //ui->stackedWidget->repaint();
    // 强制更新当前显示页面
    ui->stackedWidget->currentWidget()->repaint();
    ui->stackedWidget->currentWidget()->updateGeometry();

    // 如果页面切换还是有问题，可以尝试强制刷新整个 stackedWidget
    ui->stackedWidget->repaint();
}

void MainWindow::slot_update_dateTime(QDate date)
{
    ui->MainWindow_label_TIME_YMD->setDate(date);
}

void MainWindow::slot_updateSatellite(int num, int state)
{
    ui->MainWindow_label_bdsNum->setText(QString::number(num));
    if(state == 0){
        ui->MainWindow_label_RTK->setText("float");
    }else if(state == 4){
        ui->MainWindow_label_RTK->setText("fixed");
    }else
        ui->MainWindow_label_RTK->setText("None");
}

void MainWindow::updateDate()
{
    QDate currentDate = QDate::currentDate();
    ui->MainWindow_label_TIME_YMD->setDate(currentDate);
}
