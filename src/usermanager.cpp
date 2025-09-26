#include "usermanager.h"
#include "ui_usermanager.h"

UserManager::UserManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserManager)
{
    ui->setupUi(this);
}

UserManager::~UserManager()
{
    delete ui;
}

void UserManager::on_pbn_loadPhoto_clicked()
{
    ui->label->clear();
    // 打开文件对话框选择图片
        QString filePath = QFileDialog::getOpenFileName(
            this,
            "选择照片",
            "",
            "图像文件 (*.png *.jpg *.jpeg *.bmp *.gif)");

        if (!filePath.isEmpty()) {
            // 加载并显示图像
            QImage image(filePath);
            if (!image.isNull()) {
                ui->label->setStyleSheet("border: 1px solid #cccccc;");
                // 缩放图像以适应标签大小
                QPixmap pixmap = QPixmap::fromImage(image).scaled(
                    ui->label->size(),
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation);

                // 设置图像到标签
                ui->label->setPixmap(pixmap);
                ui->label->setText("");
            } else {
                ui->label->setText("无法加载图像");
            }
        }
}

void UserManager::on_pbn_saveUserManager_clicked()
{
    QMessageBox::information(this, "保存成功", "已将信息保存至本地", QMessageBox::Ok);
}

void UserManager::on_pbt_UserRoleModifyMode_clicked()
{
    //QMessageBox::information(this, "失败", "用户权限有限，访问失败", QMessageBox::Ok);
    QMessageBox::information(this, "成功", "权限识别通过，欢迎您，管理员", QMessageBox::Ok);
}
