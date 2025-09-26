//#include "login.h"
#include "ui_login.h"
#include "login.h"
#include "menu.h"
#include <QDebug>

login::login(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::login)
    , db(Database::instance("database_SQ.db"))
{
    ui->setupUi(this);
    moveToCenter();//窗口居中显示

    //db = Database::instance("database_SQ.db");
    this->setAttribute(Qt::WA_DeleteOnClose);           //对话框关闭时自动删除
    this->setWindowFlags(Qt::SplashScreen);             //设置为SplashScreen, 窗口无边框,不在任务栏显示
    //setWindowFlags(Qt::FramelessWindowHint);//去除标题栏
    ui->label_pwd->setScaledContents(true);//图片自适应label大小
    ui->label_user_name->setScaledContents(true);
    ui->lineE_pwd->setEchoMode(QLineEdit::Password);//设置为小黑点


    //槽函数：链接登陆按钮点击响应函数btn_login_Slot()
    connect(ui->btn_login,&QPushButton::clicked,this,&login::btn_login_Slot);
}

login::~login()
{
    delete ui;
}
//将窗口剧中显示
void login::moveToCenter()
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

// 登录按钮
void login::btn_login_Slot()
{
    bool loginState = false;
    // step 1: 获取输入框中填写的账号和密码
    QString userString = ui->lineE_user_name->text();
    QString pwdString = ui->lineE_pwd->text();

    // step 2: 查询数据库，获取对应用户名的密码
    QString storedPassword = db.queryPasswordByUsername(userString);  // 使用你之前实现的查询函数

    // step 3: 判断密码是否匹配
    if (!storedPassword.isEmpty() && storedPassword == pwdString) {
        loginState = true;
        QMessageBox::information(this, "登陆成功", "欢迎使用智能农机终端", QMessageBox::Ok);
        this->accept();  // 登录成功，关闭对话框
    }

    // step 4: 登录失败，显示提示消息
    if (!loginState) {
        QMessageBox::warning(this, "登陆失败", "用户名或密码错误", QMessageBox::Ok);
    }
}

// 注册按钮
void login::on_btn_enroll_clicked()
{
    QString userString = ui->lineE_user_name->text();
    QString pwdString = ui->lineE_pwd->text();
    bool enrollState = db.addUserData_NewUser(userString,pwdString);
    if(enrollState){
        QMessageBox::information(this, "注册成功", "请登录农机终端", QMessageBox::Ok);
    }
    else{
        QMessageBox::warning(this, "注册失败", "请联系管理员", QMessageBox::Ok);
    }
}

void login::on_btn_canel_clicked()
{
    this->reject(); //reject(), 关闭对话框
}

void login::mousePressEvent(QMouseEvent *event)
{ //鼠标按键被按下
    if (event->button() == Qt::LeftButton)
    {
        m_moving = true;
        m_lastPos = event->globalPos()- this->pos();
    }
    return QDialog::mousePressEvent(event);
}

void login::mouseMoveEvent(QMouseEvent *event)
{ //鼠标按下左键移动
    QPoint eventPos=event->globalPos();

    if (m_moving && (event->buttons() & Qt::LeftButton)
        && (eventPos-m_lastPos).manhattanLength() > QApplication::startDragDistance())
    {
        move(eventPos-m_lastPos);
        m_lastPos = eventPos - this->pos();
    }
    return QDialog::mouseMoveEvent(event);
}

void login::mouseReleaseEvent(QMouseEvent *event)
{ //鼠标按键被释放
    m_moving=false;     //停止移动
    event->accept();
}
