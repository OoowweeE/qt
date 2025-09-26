//#include "menu.h"
#include "ui_menu.h"
#include "menu.h"
#include "login.h"
#include "PoseData.hpp"

menu::menu(int argc, char** argv,QWidget *parent) :
    QMainWindow(parent),work1(argc,argv,parent),setting1(argc,argv,parent),
    ui(new Ui::menu)
{
    ui->setupUi(this);
    moveToCenter();
    //move(100,100);
    //setWindowFlags(Qt::FramelessWindowHint);//去除标题栏
    connect(ui->menu_btn_login,&QPushButton::clicked,this,&menu::menu_btn_login_slot);    
    connect(ui->menu_btn_work,&QPushButton::clicked,this,&menu::menu_btn_work_slot);

}

menu::~menu()
{
    delete ui;
}

void menu::moveToCenter()
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

void menu::menu_btn_login_slot()
{
    //this->hide();
    login *login1 = new login();
    login1->show();
}

void menu::menu_btn_work_slot()
{
    //this->hide();
    work1.show();
}

void menu::on_menu_btn_setting_clicked()
{
    //this->hide();
    setting1.show();

}

void menu::on_menu_btn_history_clicked()
{
    //this->hide();
    pathPlan1.show();
}
