#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QWidget>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QMessageBox>

namespace Ui {
class UserManager;
}

class UserManager : public QWidget
{
    Q_OBJECT

public:
    explicit UserManager(QWidget *parent = nullptr);
    ~UserManager();

private slots:
    void on_pbn_loadPhoto_clicked();

    void on_pbn_saveUserManager_clicked();

    void on_pbt_UserRoleModifyMode_clicked();

private:
    Ui::UserManager *ui;
};

#endif // USERMANAGER_H
