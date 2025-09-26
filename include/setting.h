
#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include <QPainter>
#include <QProxyStyle>
#include <QStyleOptionTab>

#include <QMessageBox>
#include "qnode.hpp"
#include "terminal.h"
#include <QDebug>
#include "sqqfile.h"
#include "serialport.h"
#include <QDateTime>
#include <QTimer>
#include <QTime>
#include <QLineEdit>
#include "database.hpp"
#include "MachineStatus.hpp"
#include "CroplandInfo.hpp"
#include "YieldData.hpp"

#include "usermanager.h"    //用户信息管理
#include "adminmangager.h"


#define UARTCONNECT       1
#define UARTUNCONNECT   0

#define temp_biye_zigbee


QT_BEGIN_NAMESPACE
namespace Ui { class Setting; }
QT_END_NAMESPACE

using namespace ros_qt_demo;

class Setting : public QWidget

{
    Q_OBJECT

public:
    //Setting(QWidget *parent = nullptr);
    explicit Setting(int argc=0, char** argv=nullptr,QWidget *parent = nullptr,QNode *RosNode = nullptr);
    ~Setting();

    void showNoMasterMessage();//ros连接失败提示

    void moveToCenter();//移动到屏幕中心位置


    //void getGNSSTransform(bool state);

    void UartConnect(bool connectState);

    void SerialSendData_SQ(QLineEdit *lineEdit);
    void SerialSendChar_SQ(QString stringChar);
    void SerialSendString_SQ(QString stringChar);


    QString structToQString(const ANetData_t& data);

    // 点击Setting->基础参数配置->保存信息
    bool updateBaseInfoInfo();

    bool QProcess_change_resolution(int width, int height);//调整屏幕分辨率
    void QProcess_change_theme(int index);//调整主题
    bool QProcess_change_theme(const QString &styleSheetPath);
    QString getStyleSetting(const QString &fileName);
    QString structToNaviQString(const ANetData_t &data);
    QString structToHarvestQString(const ANetData_t &data);
protected:
    void keyPressEvent(QKeyEvent *event) override;//重写按键操作
signals:
    //目前有个问题，在不同的页面ros_connect的时候，由于是在不同的页面ros_Connect，所以各自节点互相独立
    //因此应该修改setting里的函数，让他应该是点击发送的时候设置一个标志位保存，每次创建这个对象时在其初始化中都对这个标志位进行判断
    //根据标志4位判断是否连接ros
    void signal_update_fix_1(float,float,float);
    void signal_update_final_path_1(float,float);
    void signal_update_velSpeed_1(float,float);
    //void globalSignal_KeyPress(); //全局信号也是定义在全局信号文件中的

    void signal_Serial_SendData(QByteArray data);
    void signal_dateTimeChange(QDate date);
public slots:
    //void slot_Ros_Start_And_Conncet(bool state);
    void slot_Cat1_Start_And_Conncet(bool state);
    void updateLoggingView();
private slots:
    // 勾选定时发送后的响应槽函数
    void TimeSendReady(int state);
    //4GCat1定时上传频率
    void slot_TimerOut_4GCat1();

    //com发送数据的接收槽函数
    void slot_Serial_RcvData(QByteArray RecvBuff);
    //打开与Ros的连接
    void on_btn_rosConnect_clicked();
    //断开Ros的连接
    void on_btn_rosDisConnect_clicked();
    //是否勾选了【使用环境变量】
    void on_checkbox_useEnvironment_stateChanged(int state);

    //与node相关的，获取话题数据
    void slot_update_fix_1(float,float,float);
    void slot_update_finalPath_1(float,float);
    void slot_update_velSpeed_1(float,float);
    void slot_update_yaw(float);
    void slot_update_roll(float);
    void slot_update_pitch(float);
    void slot_update_harvest(QString);



    void on_ptb_openFile_clicked();



    void on_ptb_openFile2_clicked();


    void on_ptb_changeXML_clicked();

    //void on_ptb_GNSSTransform_clicked();

    //void on_pushButton_21_clicked();

    //void on_ptb_openRosbag_clicked();

    void on_OpenUart_clicked();

    void on_SendDataFile_clicked();

    void on_ptb_QMTCFG_clicked();

    void on_ptb_QMTCFG_2_clicked();

    void on_ptb_QMTOPEN_clicked();

    void on_ptb_QMTCONN_clicked();

    void on_ptb_QMTPUB_clicked();

    void on_ptb_QMTPUB_content_clicked();

    void on_ptb_QMTSUB_clicked();

    void on_ptb_QMTUNS_clicked();

    void on_ptb_Cat1ConnetSerial_clicked();



    void on_ptb_getConnCat1_clicked();

    void on_ptb_testCat1_clicked();

    void on_ptb_StopCat1_clicked();

    void on_RefreshUart_clicked();

    void on_CloseUart_clicked();

    void on_ClearRecvButton_clicked();

    void on_pbt_saveBaseInfo_clicked();

    void on_pbt_initBaseInfo_clicked();

    void on_pbt_loadBaseInfo_clicked();

    void on_pbt_checkBaseInfo_clicked();

    void on_checkBox_clicked();

    void on_checkBox_2_clicked();

    void on_checkBox_3_clicked();

    void on_DateTimeEdit_dateChanged(const QDate &date);

    void on_fenbianlvComboBox_currentIndexChanged(int index);

    void on_ComboBox_2_currentIndexChanged(int index);

    void on_checkBox_4_stateChanged(int arg1);

    void on_checkBox_5_stateChanged(int arg1);

    void on_checkBox_7_stateChanged(int arg1);

    void on_checkBox_8_stateChanged(int arg1);

    void on_pbn_send2zigbee_clicked();


    void on_pbn_IoT_control_clicked();

private:
    Ui::Setting *ui;
    QNode *qnode;
    Terminal *terminal1;
    sqQFile *sqfile1;
    QProcess *Process_roscore;
    QProcess *Process_GNSSTransform;
    bool roscoreState = 0;



    ComSerialPort *Serial;
    bool uartState;
    QString     SendTextStr;
    QByteArray  SendTextByte;            // 串口发送数据
    quint32     dataRxNumber = 0 ;      // 记录Rx数据量
    quint8      Serial_State = 0 ;            // 串口状态
    QTimer *time_4GCat1;
    QTimer *timerSend;


    MachineStatus &MachineStatus_;
    CroplandInfo &CroplandInfo_;
    Database& db;

    UserManager *userManagerWidget;
    adminMangager *adminManagerWidget;

    bool state_IoT_navi = 0;
    bool state_IoT_harvest = 0;
    bool state_IoT_start = 0;
    bool state_Iot_stop = 0;
    bool state_IoT_send = 0;

//    float longitude_;
//    float latitude_;
//    float altitude_;
//    float yaw_;
//    float Pitch_;
//    float speed_;
};



class CustomTabStyle : public QProxyStyle
{
public:
    /* sizeFromContents
     *  用于设置Tab标签大小
     * 1.获取原标签大小
     * 2.宽高切换
     * 3.强制宽高
     * 4.return
     * */
//    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
//                           const QSize &size, const QWidget *widget) const
//    {
//        QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);

//        if (type == QStyle::CT_TabBarTab) {
//            // 获取父容器的宽度（这里假设 widget 是 QTabWidget）
//            int parentWidth = widget ? widget->width() : 200;

//            // 动态调整标签的宽度为父容器宽度的1/3
//            s.rwidth() = parentWidth / 3;
//            s.rheight() = 55;  // 保持固定高度
//        }

//        return s;
//    }

    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
                           const QSize &size, const QWidget *widget) const
    {
        QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
        if (type == QStyle::CT_TabBarTab) {
            s.transpose();
            s.rwidth() = 200; // 设置每个tabBar中item的大小
            s.rheight() = 55;
        }
        return s;//最终返回了一个QSize类型的数据结构，等下传进
    }

    /* drawControl
     *  画控件
     * 1.过滤CE_TabBarTabLabel
     * 2.选择状态下的背景绘制
     *      a.获取Rect
     *      b.保存旧的画笔
     *      c.设置新画笔
     *      d.画 Rect
     *      e.恢复旧的画笔
     * 3.设定Text对齐及选中/非选中状态下的Text颜色
     * 4.过滤CE_TabBarTab，对其进行绘制
     * */
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
    {
        if (element == CE_TabBarTabLabel) {
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option))
            {
                QRect allRect = tab->rect;

                if (tab->state & QStyle::State_Selected) {
                    painter->save();
                    painter->setPen(0x205527);
                    painter->setBrush(QBrush(0x205527));
                    QRect nrct = allRect.adjusted(6, 6, -6, -6);
                    painter->drawRect(nrct);
                    painter->restore();
                }
                QTextOption option;
                option.setAlignment(Qt::AlignCenter);
                if (tab->state & QStyle::State_Selected) {
                    painter->setPen(0xf8fcff);

                }
                else {
                    painter->setPen(0x5d5d5d);
                }

                painter->drawText(allRect, tab->text, option);
                return;
            }
        }

        if (element == CE_TabBarTab) {
            QProxyStyle::drawControl(element, option, painter, widget);
        }
    }
};


#endif // SETTING_H
