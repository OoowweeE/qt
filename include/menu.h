#ifndef MENU_H
#define MENU_H

#include <QMainWindow>
#include "work.h"
#include <QScreen>
#include "pathplan.h"
#include "setting.h"
namespace Ui {
class menu;
}

class menu : public QMainWindow
{
    Q_OBJECT

public:
    explicit menu(int argc, char** argv,QWidget *parent = nullptr);
    ~menu();

    void moveToCenter();
public slots:
    void menu_btn_login_slot();
    void menu_btn_work_slot();
private slots:
    void on_menu_btn_setting_clicked();

    void on_menu_btn_history_clicked();

private:
    Ui::menu *ui;
    work work1;
    Setting setting1;
    pathPlan pathPlan1;
};

#endif // MENU_H
