#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QDialog>
//#include "sqorm.h"
#include <QMessageBox>
#include <QDesktopWidget>
#include <QMouseEvent>
#include "database.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class login; }
QT_END_NAMESPACE

class login : public QDialog
{
    Q_OBJECT

public:
    login(QWidget *parent = nullptr);
    ~login();
    void moveToCenter();
protected:
//鼠标事件处理函数，用于拖动窗口
     void mousePressEvent(QMouseEvent *event);
     void mouseMoveEvent(QMouseEvent *event);
     void mouseReleaseEvent(QMouseEvent *event);
private:
     bool    m_moving=false;     //表示窗口是否在鼠标操作下移动
     QPoint  m_lastPos;          //上一次的鼠标位置
public slots:
    void btn_login_Slot();

private slots:
    void on_btn_enroll_clicked();

    void on_btn_canel_clicked();

private:
    Ui::login *ui;
    Database& db;
};
#endif // LOGIN_H
