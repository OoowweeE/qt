
#include "setting.h"
#include "ui_setting.h"
#include <QScreen>
#include <setting.h>
#include "globalsignal.h"


//Setting::Setting(QWidget *parent)
//    : QWidget(parent)
//    , ui(new Ui::Setting)
//{
//    ui->setupUi(this);

//    ui->tabWidget->tabBar()->setStyle(new CustomTabStyle);

//}

Setting::Setting(int argc, char **argv, QWidget *parent,QNode *RosNode)
    :QWidget(parent),ui(new Ui::Setting),
      db(Database::instance("database_SQ.db")),
      MachineStatus_(MachineStatus::instance()),
      CroplandInfo_(CroplandInfo::instance())
{

    ui->setupUi(this);
    //qnode = RosNode;
    qnode = new QNode(argc,argv);
    ui->sensorManager_label_longitude->adjustSize();

    // 新建用户信息页对象并绑定至UserWidget_QVBoxLayout页面
    userManagerWidget = new UserManager();
    ui->UserWidget_QVBoxLayout->addWidget(userManagerWidget);

    adminManagerWidget = new adminMangager();
    ui->UserWidget_QVBoxLayout->addWidget(adminManagerWidget);

    moveToCenter();//窗口居中显示
    time_4GCat1 = new QTimer(this);
    timerSend = new QTimer(this);
    sqfile1 = new sqQFile(this);
    ui->tabWidget->tabBar()->setStyle(new CustomTabStyle);


    //将terminal放到setting里
    terminal1 = new Terminal(this);
    //QWidget *terminalCentralWidget = terminal1->centralWidget();
    ui->vLayout_terminal->addWidget(terminal1->centralWidget());


    QObject::connect(qnode, SIGNAL(rosShutdown()), this, SLOT(close()));
    QObject::connect(qnode,SIGNAL(fix_vel(float,float,float)),this,SLOT(slot_update_fix_1(float,float,float)));
    QObject::connect(qnode,SIGNAL(finalPathXY_vel(float,float)),this,SLOT(slot_update_finalPath_1(float,float)));
    QObject::connect(qnode,SIGNAL(velSpeed_vel(float,float)),this,SLOT(slot_update_velSpeed_1(float,float)));
    QObject::connect(qnode,SIGNAL(yaw_vel(float)),this,SLOT(slot_update_yaw(float)));
    QObject::connect(qnode,SIGNAL(roll_vel(float)),this,SLOT(slot_update_roll(float)));
    QObject::connect(qnode,SIGNAL(pitch_vel(float)),this,SLOT(slot_update_pitch(float)));
    QObject::connect(qnode,SIGNAL(harvestData_CAN(QString)),this,SLOT(slot_update_harvest(QString)));

    QObject::connect(time_4GCat1,&QTimer::timeout,this,&Setting::slot_TimerOut_4GCat1);

    QObject::connect(timerSend, SIGNAL(timeout()), this, SLOT(on_SendDataFile_clicked()));
    QObject::connect(ui->timesendcheckBox,SIGNAL(stateChanged(int)),this,SLOT(TimeSendReady(int)));



    Process_roscore = new QProcess; //Process_1->start("bash");
    Process_GNSSTransform = new QProcess;

    UartConnect(0);
    ui->CloseUart->setEnabled(false);       // btn串口关闭禁用

    /*********************
    ** Logging
    **********************/
    ui->view_logging->setModel(qnode->loggingModel());
    QObject::connect(qnode, SIGNAL(loggingUpdated()), this, SLOT(updateLoggingView()));
    ui->view_logging->setWordWrap(true);

    ui->RecvListView->setModel(qnode->loggingModel());
    ui->RecvListView->setWordWrap(true);

}


Setting::~Setting()
{
    delete ui;
}

//将窗口中显示
void Setting::moveToCenter()
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

void Setting::showNoMasterMessage()
{

    QMessageBox msgBox;
    msgBox.setText("Couldn't find the ros master.");
    msgBox.exec();
    close();
}


void Setting::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_F1) {
        // setting页面监听到按键事件后
        emit GlobalSignal::instance()->globalSignal_KeyPress();//全局变量发送信号——x和y坐标值
        //qDebug() << "按键event";
    } else {
        QWidget::keyPressEvent(event);  // 处理其他按键事件
    }
}

void Setting::on_btn_rosConnect_clicked()
{
//    showNoMasterMessage();
    //首先判断是否勾选了【使用环境变量】
    if ( ui->checkbox_useEnvironment->isChecked() ) {
        //如果勾选了，判断qnode.init()是否成功，不成功就弹出报错窗口
        if ( !qnode->init() ) {
            showNoMasterMessage();
            QMessageBox::warning(this,tr("错误"),tr("roscore未启动，连接中断"));
        } else {
            ui->btn_rosConnect->setEnabled(false);
            QMessageBox::information(this,tr("已连接"),tr("连接成功"));
        }
    } else {//如果没勾选环境变量，就根据输入内容自定义初始化连接
        if ( ! qnode->init(ui->line_edit_master->text().toStdString(),
                   ui->line_edit_host->text().toStdString()) ) {
            showNoMasterMessage();//如果连接失败，弹出报错窗口
        } else {//成功了就执行下面
            ui->btn_rosConnect->setEnabled(false);
            ui->line_edit_master->setReadOnly(true);
            ui->line_edit_host->setReadOnly(true);
            ui->line_edit_topic->setReadOnly(true);
        }
    }
}



void Setting::on_checkbox_useEnvironment_stateChanged(int state)
{
    bool enabled;
    if ( state == 0 ) {
        enabled = true;
    } else {
        enabled = false;
    }
    ui->line_edit_master->setEnabled(enabled);
    ui->line_edit_host->setEnabled(enabled);
}

void Setting::slot_update_fix_1(float longitude,float latitude,float altitude)
{
    //qDebug()<<"signal_update_fix_1 正在发射" << endl;
    //emit signal_update_fix_1(longitude,latitude,altitude);
    emit GlobalSignal::instance()->globalSignal_update_FixGNSS(longitude,latitude,altitude);//全局变量发送信号——
    //更新数据至采集传感器界面中
//    longitude_ = longitude;
//    latitude_  = latitude;
//    altitude_  = altitude;
    ui->sensorManager_label_longitude->setText(QString::number(longitude,'f',6));
    ui->sensorManager_label_latitude->setText(QString::number(latitude,'f',6));
    ui->sensorManager_label_elevation->setText(QString::number(altitude,'f',6));
    //qDebug()<<"signal_update_fix_1 发射成功" << endl;
}

void Setting::slot_update_finalPath_1(float x, float y)
{
    //emit signal_update_final_path_1(x,y);
    emit GlobalSignal::instance()->globalSignal_update_finalPathXY(x,y);//全局变量发送信号——x和y坐标值
       // qDebug()<<"slot_update_finalPath_1 发射成功" << endl;
}

void Setting::slot_update_velSpeed_1(float x, float y)
{
    //x线速度和y线速度
    emit GlobalSignal::instance()->globalSignal_update_velSpeed(x,y);//全局变量发送信号——
}

void Setting::slot_update_yaw(float yaw)
{
    ui->sensorManager_label_Yaw->setText(QString::number(yaw,'f',6));
}

void Setting::slot_update_roll(float roll)
{
    ui->sensorManager_label_Roll->setText(QString::number(roll,'g',6));
}

void Setting::slot_update_pitch(float pitch)
{
    ui->sensorManager_label_Pitch->setText(QString::number(pitch,'g',6));
}

void Setting::slot_update_harvest(QString canString)
{
    QStringList canStringList = canString.split(",");
    QString can_ID = canStringList[0];
    QString can_Data = canStringList[1];
    if(can_ID == "419366615"){
        float can_totalYield = YieldData::HarvestID1_totalYield(can_Data);
        //ui->work_label_totalYield->setText(QString::number(can_totalYield, 'f', 3));
        float speedXY = YieldData::instance().canData.speed;
        //can_actual_per_Acre_Yield = can_totalYield / 5 /speedXY / 666.67;
        //ui->work_label_realTimeYield->setText(QString::number(can_actual_per_Acre_Yield, 'f', 3));
    }else if(can_ID == "419365450"){
        float can_grainMoisture = YieldData::HarvestID2_grainMoisture(can_Data);
        ui->sensorManager_label_water->setText(QString::number(can_grainMoisture, 'f', 3));
    }else if(can_ID == "419366614"){
        float can_grainFlowRate = YieldData::HarvestID3_grainFlowRate(can_Data);
        ui->sensorManager_label_yield->setText(QString::number(can_grainFlowRate, 'f', 6));
    }
}




void Setting::on_btn_rosDisConnect_clicked()
{
    qnode->shutdownRos();
    ui->btn_rosConnect->setEnabled(true);
    ui->line_edit_master->setReadOnly(false);
    ui->line_edit_host->setReadOnly(false);
    ui->line_edit_topic->setReadOnly(false);
}

void Setting::on_ptb_openFile_clicked()
{
    sqfile1->openFileDialog(this);
    ui->lineEdit_filePath->setText(sqfile1->filePath);
}



void Setting::on_ptb_openFile2_clicked()
{
    ui->textEdit_fileContent->clear();
    sqfile1->openFile(sqfile1->filePath);
    for (const QString &line : sqfile1->fileContent) {
        ui->textEdit_fileContent->append(line);
    }
}



void Setting::on_ptb_changeXML_clicked()
{
    sqfile1->modifyXml(sqfile1->filePath,
                       ui->lineEdit_tagName->text(),
                       ui->lineEdit_tagNameValue->text(),
                       ui->lineEdit_attributeName->text(),
                       ui->lineEdit_attributeNameValue->text());
    on_ptb_openFile2_clicked();
}


//void Setting::getGNSSTransform(bool state)
//{
//    if(state == 0){
//        Process_GNSSTransform->start("bash");
//        QString processStringBashrc = "source ~/ws/devel/setup.bash";
//        //执行命令前先source一下
//        Process_GNSSTransform->write(processStringBashrc.toUtf8()+'\n');
//        QString processStringGNSSTransform = "roslaunch nmea_navsat_driver nmea_serial_driver_posture_pose.launch";
//        //订阅GNSS信息并发布位置信息
//        Process_GNSSTransform->write(processStringGNSSTransform.toUtf8()+'\n');//
//        ui->ptb_GNSSTransform->setText("转换成功");
//    }else{//state == 1时关闭roscore
//        //杀死进程
//        Process_GNSSTransform->write(QString(QChar(0x03)).toUtf8());
//        Process_GNSSTransform->kill();
//        Process_GNSSTransform->close();
//        //重启终端
//        Process_GNSSTransform->start("bash");
//        qDebug()<<"roscore已关闭";
//        ui->ptb_GNSSTransform->setText("GNSS转换");
//    }
//}

void Setting::UartConnect(bool connectState)
{
    if(connectState == 1){
        qint32 state = 0;
        //初始化串口"/dev/ttyUSB0"
        //Serial   = new ComSerialPort(ui->uartbox->currentText(),ui->baudbox->currentText().toInt(),&state);
        QString  stringUartName = ui->uartbox->currentText();
        int intBaud = ui->baudbox->currentText().toInt();
//
        Serial = new ComSerialPort(stringUartName,intBaud,&state);//Serial   = new ComSerialPort("ttyUSB2",115200,&state);
        if(state){//串口打开成功
            ui->OpenUart->setEnabled(false);
            ui->CloseUart->setEnabled(true);
            Serial_State = 1;//当前串口处于开启状态
            QMessageBox::warning(this,tr("提示"),tr("串口连接成功"));
            //绑定：setting发送-com接收
            connect(this,&Setting::signal_Serial_SendData,Serial,&ComSerialPort::SendSerialData,Qt::AutoConnection);
            //绑定：com发送-setting接收
            connect(Serial,SIGNAL(UpdateData(QByteArray)),this,SLOT(slot_Serial_RcvData(QByteArray)),Qt::AutoConnection);
        }
        else{
            Serial_State = 0;
            QMessageBox::warning(this,tr("错误"),tr("串口连接失败"));
        }
        Serial->getSerialPort();
    }
    else if(connectState == 0){
        Serial_State = 0;
        if(Serial != nullptr)//这里需要进行Serial是否初始化的判断,防止出问题
            Serial->CloseSerial();//关闭串口
    }
}

void Setting::slot_Serial_RcvData(QByteArray RecvBuff)
{
    QString     stringdata;
    ui->RecvTextEdit->setTextColor(QColor(Qt::black));
    // 读取串口接收的数据
    if(ui->recvcheckBox->isChecked()){
        stringdata = RecvBuff.toHex(' ').trimmed().toUpper();/*hex显示*/
    }
    else{
       stringdata = QString(RecvBuff);   /*ascii显示*/
    }
    /*时间戳按钮*/
    if (ui->timecheckBox->isChecked()){
       stringdata = QString("[%1]:RX -> %2").arg(QTime::currentTime().toString("HH:mm:ss:zzz")).arg(stringdata);
       ui->RecvTextEdit->append(stringdata);
    }
    else{
        ui->RecvTextEdit->insertPlainText(stringdata);
    }
    ui->RecvTextEdit->moveCursor(QTextCursor::End);  // 自动滚屏到最后一行 有BUG 不可用光标点击文本框
    dataRxNumber += RecvBuff.length();
    ui->RxNumlabel->setText(QString::number(dataRxNumber));
    RecvBuff.clear();
}


//void Setting::on_ptb_GNSSTransform_clicked()
//{
//    getGNSSTransform(0);//传入0，开始执行GNSS转换
//}

//void Setting::on_pushButton_21_clicked()
//{
//    getGNSSTransform(1);
//}

//void Setting::on_ptb_openRosbag_clicked()
//{

//}

void Setting::on_OpenUart_clicked()
{
    UartConnect(1);
}

void Setting::on_SendDataFile_clicked()
{
    if (Serial_State == 1){
        //获取发送框字符
        SendTextStr = ui->SendTextEdit->document()->toPlainText();
        SendTextByte = SendTextStr.toUtf8();
        if (SendTextByte.isEmpty() != true){
            SendTextByte+="\r\n";
            if (ui->HexsendcheckBox->isChecked()) {
                SendTextByte = SendTextByte.fromHex(SendTextByte);
                emit signal_Serial_SendData(SendTextByte);
                QString strdata = SendTextByte.toHex(' ').trimmed().toUpper();
                if (ui->timecheckBox->isChecked()) { // 时间戳发送
                    ui->RecvTextEdit->setTextColor(QColor(Qt::blue));  // 时间戳颜色
                    ui->RecvTextEdit->append(QString("[%1]TX -> ").arg(QTime::currentTime().toString("HH:mm:ss:zzz")));
                }
                else{
                  strdata = strdata.append("\r\n");
                }
                ui->RecvTextEdit->setTextColor(QColor(Qt::black));
                ui->RecvTextEdit->insertPlainText(strdata);
            }
            else{
                // 发送ascii数据
                //SendTextByte = SendTextByte +"\r\n";
                emit signal_Serial_SendData(SendTextByte);
                QString strdata = QString(SendTextByte);
                if (ui->timecheckBox->isChecked()) { // 时间戳发送
                    ui->RecvTextEdit->setTextColor(QColor(Qt::red)); // 时间戳颜色
                    ui->RecvTextEdit->append(QString("[%1]TX -> ").arg(QTime::currentTime().toString("HH:mm:ss:zzz")));
                }
                else{
                   strdata = strdata.append("\r\n");
                }
                ui->RecvTextEdit->setTextColor(QColor(Qt::black));
                ui->RecvTextEdit->insertPlainText(strdata);
            }
            //移动光标到末尾
            ui->RecvTextEdit->moveCursor(QTextCursor::End);
        }
        else{
            QMessageBox::warning(this, "警告", "您需要在发送编辑框中输入要发送的数据");
        }

    }
    else{
        QMessageBox::information(this, "警告", "串口未打开");
    }
}

//传入lineEdit中的值，将值发出去
void Setting::SerialSendData_SQ(QLineEdit *lineEdit){

    if (Serial_State == 1){
        QString string4GCAT1 = lineEdit->text();
        QByteArray  byteArray4GCAT1 = string4GCAT1.toUtf8();
        byteArray4GCAT1 += "\r\n";
        qDebug() << string4GCAT1;
        qDebug() << byteArray4GCAT1;
        emit signal_Serial_SendData(byteArray4GCAT1);
    }
    else{
        QMessageBox::information(this, "警告", "串口未打开");
    }
}

void Setting::SerialSendString_SQ(QString stringChar){
    if (Serial_State == 1){
        QByteArray  byteArray = stringChar.toUtf8();
        byteArray += "\r\n";
        //byteArray4GCAT1 = byteArray4GCAT1.fromHex(byteArray4GCAT1);
        //qDebug() << string4GCAT1;
        qDebug() << byteArray << " " << stringChar;
        emit signal_Serial_SendData(byteArray);
    }
    else{
        QMessageBox::information(this, "警告", "串口未打开");
    }
}

//发送字符，传入“0x1a”
void Setting::SerialSendChar_SQ(QString stringChar){
    if (Serial_State == 1){
        QByteArray  byteArray4GCAT1 = stringChar.toUtf8();
        byteArray4GCAT1 = byteArray4GCAT1.fromHex(byteArray4GCAT1);
        //qDebug() << string4GCAT1;
        qDebug() << byteArray4GCAT1;
        emit signal_Serial_SendData(byteArray4GCAT1);
    }
    else{
        QMessageBox::information(this, "警告", "串口未打开");
    }
}

//MQTT连接
void Setting::on_ptb_QMTCFG_clicked()//参数配置
{
    SerialSendData_SQ(ui->lineEdit_QMTCFG);
}

void Setting::on_ptb_QMTCFG_2_clicked()//配置认证
{
    SerialSendData_SQ(ui->lineEdit_QMTCFG_2);
}

void Setting::on_ptb_QMTOPEN_clicked()//打开网络
{
    SerialSendData_SQ(ui->lineEdit_QMTOPEN);
}

void Setting::on_ptb_QMTCONN_clicked()//连接网络
{
    SerialSendData_SQ(ui->lineEdit_QMTCONN);
}


void Setting::on_ptb_QMTPUB_clicked()//主题绑定
{
    SerialSendData_SQ(ui->lineEdit_QMTPUB);
}

void Setting::on_ptb_QMTPUB_content_clicked()//发布消息
{
    SerialSendData_SQ(ui->lineEdit_QMTPUB_content);
    SerialSendChar_SQ("0x1a");
    int timer_SettingTime = ui->lineEdit_TimerSetTime->text().toInt();
    time_4GCat1->start(timer_SettingTime);
}

void Setting::on_ptb_QMTSUB_clicked()//主题订阅
{
    SerialSendData_SQ(ui->lineEdit_QMTSUB);
}

void Setting::on_ptb_QMTUNS_clicked()//主题退订
{
    SerialSendData_SQ(ui->lineEdit_QMTUNS);
}

void Setting::slot_TimerOut_4GCat1(){
#ifdef temp_biye_zigbee
    if(state_IoT_navi == 1){
        QString naviString;
        naviString = structToNaviQString(globalValue::ANetData);
        //补解析操作
        SerialSendString_SQ(naviString);
    }
    if(state_IoT_harvest == 1){
        QString harvestString;
        //补解析操作
        harvestString = structToHarvestQString(globalValue::ANetData);
        SerialSendString_SQ(harvestString);
    }
#else
    QString outputData;outputData=structToQString(globalValue::ANetData);
    qDebug() << "倒计时已到，数据上传云平台："<<outputData;
    SerialSendData_SQ(ui->lineEdit_QMTPUB);//指定发送所指向的主题
    SerialSendString_SQ(outputData);
    SerialSendChar_SQ("0x1a");
#endif
}

QString Setting::structToQString(const ANetData_t& data) {
    // 使用 QString::arg() 格式化字符串并拼接
    QString result = QString("%1"
                             ",%2"
                             ",%3"
                             ",%4,%5"
                             ",%6,%7"
                             ",%8,%9"
                             ",%10,%11")
                             .arg(data.modelVersion)           // 型号版本
                             .arg(data.deviceNumber)            // 设备号
                             .arg(data.statusCode)              // 状态码
                             .arg(data.latitude, 0, 'f', 6)     // 纬度，保留6位小数
                             .arg(data.latitudeUnit)            // 纬度单位 (N/S)
                               .arg(data.longitude, 0, 'f', 6)    // 经度，保留6位小数
                             .arg(data.longitudeUnit)           // 经度单位 (E/W)
                             .arg(data.altitude, 0, 'f', 2)     // 高度，保留2位小数
                             .arg(data.altitudeUnit)           // 高度单位 (m/ft)
                             .arg(data.speed,0,'f',6)
                             .arg(data.speddUnit);

    return result;
}

QString Setting::structToNaviQString(const ANetData_t& data) {
    // 使用 QString::arg() 格式化字符串并拼接
    QString NaviTime = QString("0520") + (QTime::currentTime().addSecs(-8*3600).toString("hhmmss"));
    qDebug()<<"time:" << NaviTime;
    QString result = QString(
                "$0,N,%1,%2N,%3E,%4m,%5,%6,%7,%8m/s"
                             ).arg(NaviTime)
                              .arg(data.latitude, 0, 'f', 6)
                              .arg(data.longitude, 0, 'f', 6)
                              .arg(data.altitude, 0, 'f', 2)
                              .arg(data.yaw,0,'f',6)
                              .arg(data.pitch,0,'f',2)
                              .arg(data.pitch,0,'f',2)
                              .arg(data.speed,0,'f',6);
    return result;
}

QString Setting::structToHarvestQString(const ANetData_t &data)
{
    // 使用 QString::arg() 格式化字符串并拼接
    QString HarvestTime = QString("0520") + (QTime::currentTime().addSecs(-8*3600).toString("hhmmss"));
    qDebug()<<"time:" << HarvestTime;
    QString result = QString(
                "$0,H,%1,%2,%3,%4,%5"
                             ).arg(HarvestTime)
                              .arg(data.guwuhanshuilv, 0, 'f', 6)
                              .arg(data.guwuliuliang, 0, 'f', 6)
                              .arg(data.shishimuchan, 0, 'f', 6)
                              .arg(data.dangqianchanliang,0,'f',6);
    return result;
}

void Setting::on_ptb_Cat1ConnetSerial_clicked()
{
    UartConnect(1);
}


//一键进行和4GCat1的连接
void Setting::on_ptb_getConnCat1_clicked()
{
#ifdef temp_biye_zigbee
    SerialSendString_SQ("connect success!");
    qDebug()<<"建立连接成功";
#else
    QTimer::singleShot(500, [this]() {
        SerialSendString_SQ("AT");//初始化一下
        on_ptb_QMTCFG_clicked();
        QTimer::singleShot(500, [this]() {
            on_ptb_QMTCFG_2_clicked();
            QTimer::singleShot(500, [this]() {
                on_ptb_QMTOPEN_clicked();
                QTimer::singleShot(500, [this]() {
                    on_ptb_QMTCONN_clicked();
                    QTimer::singleShot(500, [this]() {
                        on_ptb_QMTSUB_clicked();//订阅主题
                    });
                });
            });
        });
    });
#endif
}

void Setting::on_ptb_testCat1_clicked()
{
#ifdef temp_biye_zigbee
    SerialSendString_SQ("$0,N,0,0,0,0,0,0,0,0");
    qDebug()<<"建立连接成功";
#else
    QTimer::singleShot(500, [this]() {
        SerialSendString_SQ("AT");//初始化一下
        QTimer::singleShot(500, [this]() {
            on_ptb_QMTPUB_clicked();
            QTimer::singleShot(500, [this]() {
                on_ptb_QMTPUB_content_clicked();
            });
        });
    });
#endif
}

void Setting::on_ptb_StopCat1_clicked()
{
    time_4GCat1->stop();
}

void Setting::on_RefreshUart_clicked()
{
    ui->uartbox->clear();
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()){
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite)){
            ui->uartbox->addItem((serial.portName()));
            serial.close();
        }
    }
}

// 关闭串口按键
void Setting::on_CloseUart_clicked()
{
    if(Serial_State == 1){//先判断是否已经打开串口
        Serial->CloseSerial();
        //Serial->deleteLater();
        Serial_State = 0;
        ui->OpenUart->setEnabled(true);//连接串口按键使能
        ui->CloseUart->setEnabled(false);//关闭串口按键禁用
    }
    else{//若没有打开，报错
        QMessageBox::information(this, "警告", "串口未打开");
    }
}

void Setting::on_ClearRecvButton_clicked()
{
    ui->RecvTextEdit->clear();
    dataRxNumber = 0;
    ui->RxNumlabel->setText(QString::number(dataRxNumber));
}

void Setting::TimeSendReady(int state){
    int setTime;
    if (0 == Serial_State){
        QMessageBox::information(this, "提示", "串口未打开");
        return;
    }
    /*判断是否有数据*/
    if (ui->SendTextEdit->document()->isEmpty() == true){
        if (ui->timesendcheckBox->isChecked()){
            QMessageBox::warning(this, "警告", "您需要在发送编辑框中输入要发送的数据");
        }
        return;
    }
    /*判断勾选状态*/
    if (state == Qt::Checked){
        /*获取设定时间*/
        setTime = ui->timelineEdit->text().toInt();
        if (setTime > 0) {
            timerSend->start(setTime);
        } else {
            QMessageBox::warning(this, "警告", "时间必须大于0");
        }
        ui->timelineEdit->setEnabled(false);
    }

}
//void Setting::slot_Ros_Start_And_Conncet(bool state){
//    if(state){
//        qDebug()<<"收到1";
//        getStartRos(state);
//        //on_btn_rosConnect_clicked();
//        QTimer::singleShot(4000, [this]() {
//            //getStartRos(state);
//            on_btn_rosConnect_clicked();
//        });
//    }else{
//        //qDebug()<<"收到2";
//        getStartRos(state);
////        on_btn_rosDisConnect_clicked();
//        QTimer::singleShot(3000, [this]() {
//            //getStartRos(state);
//            on_btn_rosConnect_clicked();
//        });
////        QTimer::singleShot(1000, [this]() {
////            getStartRos(state);
////            QTimer::singleShot(1000, [this]() {
////                on_btn_rosDisConnect_clicked();
////            });
////        });
//    }
//}
void Setting::slot_Cat1_Start_And_Conncet(bool state){
    if(state){
        //qDebug()<<"收到1";
        UartConnect(state);//串口连接
        //on_ptb_getConnCat1_clicked();//配置和连接cat1
        on_ptb_testCat1_clicked();//发送测试并开始定时发送
    }else{
        //qDebug()<<"收到0";
        UartConnect(state);
        on_ptb_StopCat1_clicked();//停止定时发送
    }
}

void Setting::updateLoggingView()
{
    ui->view_logging->scrollToBottom();
    ui->RecvListView->scrollToBottom();
}



void Setting::on_pbt_saveBaseInfo_clicked()
{
    // 读取界面中输入到数值
    updateBaseInfoInfo();
    // 保存至本地数据库中
    db.addMachineStatus(MachineStatus_);
    db.updateCroplandInfo(CroplandInfo_);
    QMessageBox::information(this,"操作提示","您已成功保存数据至本地数据库");
}

bool Setting::updateBaseInfoInfo(){
    MachineStatus_.tKind_ = ui->lineEdit_tKind->text();
    MachineStatus_.tId_ = ui->lineEdit_tId->text();
    MachineStatus_.tCutWidth_ = ui->lineEdit_tCutWidth->text().toDouble();
    MachineStatus_.tMinTurningRadius_ = ui->lineEdit_tMinTurningRadius->text().toDouble();
    MachineStatus_.tMainAntennaHeight_ = ui->lineEdit_tAntennaToAxisDistance->text().toDouble();
    MachineStatus_.tAntennaToAxisDistance_ = ui->lineEdit_tAntennaToFrontAxleDistance->text().toDouble();
    MachineStatus_.tAntennaToFrontAxleDistance_ = ui->lineEdit_tAntennaToAxisDistance->text().toDouble();
    MachineStatus_.tWheelbase_ = ui->lineEdit_tWheelbase->text().toDouble();
    MachineStatus_.tFrontWheel_ = ui->lineEdit_tFrontWheel->text().toDouble();

    CroplandInfo_.driver_ = ui->lineEdit_driver->text();
    CroplandInfo_.fieldName_ = ui->lineEdit_fieldName->text();
    CroplandInfo_.fieldArea_ = ui->lineEdit_fieldArea->text().toDouble();
    CroplandInfo_.cropVariety_ = ui->lineEdit_cropVariety->text();
    CroplandInfo_.workType_ = ui->comboBox_workType->currentText();
    CroplandInfo_.soilType_ = ui->lineEdit_soilType->text();
    CroplandInfo_.weather_ = ui->comboBox_weather->currentText();
    CroplandInfo_.workDate_ = ui->lineEdit_workDate->text();
    return 1;
}




void Setting::on_pbt_initBaseInfo_clicked()
{
    ui->lineEdit_tKind->clear();
    ui->lineEdit_tId->clear();
    ui->lineEdit_tCutWidth->clear();
    ui->lineEdit_tMinTurningRadius->clear();
    ui->lineEdit_tAntennaToAxisDistance->clear();
    ui->lineEdit_tAntennaToFrontAxleDistance->clear();
    ui->lineEdit_tWheelbase->clear();
    ui->lineEdit_tFrontWheel->clear();
    ui->lineEdit_tMainAntennaHeight->clear();

    ui->lineEdit_driver->clear();
    ui->lineEdit_fieldName->clear();
    ui->lineEdit_fieldArea->clear();
    ui->lineEdit_cropVariety->clear();
    ui->comboBox_workType->clear();
    ui->comboBox_workType->addItems({" ", "耕地", "种植", "管理","收获"});
    ui->lineEdit_soilType->clear();
    ui->comboBox_weather->clear();
    ui->comboBox_weather->addItems({" ", "晴天", "阴天", "雨天","雾天"});
    ui->lineEdit_workDate->clear();
}

void Setting::on_pbt_loadBaseInfo_clicked()
{
    // 农机相关信息
    ui->lineEdit_tKind->setText(MachineStatus_.tKind_);
    ui->lineEdit_tId->setText(MachineStatus_.tId_);
    ui->lineEdit_tCutWidth->setText(QString::number(MachineStatus_.tCutWidth_));
    ui->lineEdit_tMinTurningRadius->setText(QString::number(MachineStatus_.tMinTurningRadius_));
    ui->lineEdit_tAntennaToAxisDistance->setText(QString::number(MachineStatus_.tAntennaToAxisDistance_));
    ui->lineEdit_tAntennaToFrontAxleDistance->setText(QString::number(MachineStatus_.tAntennaToFrontAxleDistance_));
    ui->lineEdit_tWheelbase->setText(QString::number(MachineStatus_.tWheelbase_));
    ui->lineEdit_tFrontWheel->setText(QString::number(MachineStatus_.tFrontWheel_));
    ui->lineEdit_tMainAntennaHeight->setText(QString::number(MachineStatus_.tMainAntennaHeight_));

    // 地块相关信息
    ui->lineEdit_driver->setText(CroplandInfo_.driver_);
    ui->lineEdit_fieldName->setText(CroplandInfo_.fieldName_);
    ui->lineEdit_fieldArea->setText(QString::number(CroplandInfo_.fieldArea_));
    ui->lineEdit_cropVariety->setText(CroplandInfo_.cropVariety_);
    ui->comboBox_workType->setCurrentText(CroplandInfo_.workType_);
    ui->lineEdit_soilType->setText(CroplandInfo_.soilType_);
    ui->comboBox_weather->setCurrentText(CroplandInfo_.weather_);
    ui->lineEdit_workDate->setText(CroplandInfo_.workDate_);
}


void Setting::on_pbt_checkBaseInfo_clicked()
{
    // 检查字符串字段是否为空
    if (MachineStatus_.tKind_.isEmpty() ||
        MachineStatus_.tId_.isEmpty() ||
        CroplandInfo_.driver_.isEmpty() ||
        CroplandInfo_.fieldName_.isEmpty() ||
        CroplandInfo_.cropVariety_.isEmpty() ||
        CroplandInfo_.workType_.isEmpty() ||
        CroplandInfo_.soilType_.isEmpty() ||
        CroplandInfo_.weather_.isEmpty() ||
        CroplandInfo_.workDate_.isEmpty()) {

        QMessageBox::warning(this, "参数校验失败", "请保存");
    }

    // 检查 double 字段是否为0
    else if (MachineStatus_.tCutWidth_ == 0 ||
        MachineStatus_.tMinTurningRadius_ == 0 ||
        MachineStatus_.tMainAntennaHeight_ == 0 ||
        MachineStatus_.tAntennaToAxisDistance_ == 0 ||
        MachineStatus_.tAntennaToFrontAxleDistance_ == 0 ||
        MachineStatus_.tWheelbase_ == 0 ||
        MachineStatus_.tFrontWheel_ == 0 ||
        CroplandInfo_.fieldArea_ == 0) {

        QMessageBox::warning(this, "参数校验失败", "请保存");
    }
    else{
        QMessageBox::information(this, "参数校验成功", "所有参数有效！");
    }
    // 所有校验通过

}

void Setting::on_checkBox_clicked()
{
    ui->comboBox_6->setCurrentIndex(0);
}

void Setting::on_checkBox_2_clicked()
{
    ui->comboBox_6->setCurrentIndex(1);
}

void Setting::on_checkBox_3_clicked()
{
    ui->comboBox_6->setCurrentIndex(2);
}

void Setting::on_DateTimeEdit_dateChanged(const QDate &date)
{
    emit signal_dateTimeChange(date);
}

// 使用xrandr命令调整Linux屏幕分辨率
bool Setting::QProcess_change_resolution(int width, int height) {
    // 获取当前显示器名称
    QProcess xrandrProc;
    xrandrProc.start("xrandr", QStringList() << "--current");
    xrandrProc.waitForFinished();

    QString output = xrandrProc.readAllStandardOutput();
    QString displayName;

    // 解析输出获取主显示器名称
    QRegularExpression re("(\\S+) connected");
    QRegularExpressionMatch match = re.match(output);
    if (match.hasMatch()) {
        displayName = match.captured(1);
    } else {
        qDebug() << "无法确定显示器名称";
        return false;
    }

    // 检查分辨率是否可用
    QProcess checkProc;
    checkProc.start("xrandr", QStringList() << "--query");
    checkProc.waitForFinished();

    QString checkOutput = checkProc.readAllStandardOutput();
    QString resolution = QString("%1x%2").arg(width).arg(height);

    if (!checkOutput.contains(resolution)) {
        qDebug() << "不支持的分辨率:" << resolution;
        return false;
    }

    // 应用新分辨率
    QProcess setProc;
    setProc.start("xrandr", QStringList() << "--output" << displayName << "--mode" << resolution);
    setProc.waitForFinished();

    return setProc.exitCode() == 0;
}

void Setting::on_fenbianlvComboBox_currentIndexChanged(int index)
{
    if(index == 1){
        QProcess_change_resolution(1600,900);
        qDebug()<<"修改分辨率为1600x900";
    }else if(index == 0){
        QProcess_change_resolution(1920,1080);
        qDebug()<<"修改分辨率为1920x1080";
    }else if(index == 2){
        QProcess_change_resolution(1280,800);
        qDebug()<<"修改分辨率为1280x800";
    }
}

// 导入样式表
QString Setting::getStyleSetting(const QString &fileName){
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

/**
 * @brief 更改应用程序的全局样式表
 * @param index 样式表序号
 * @return 空
 */
void Setting::QProcess_change_theme(int index){
    QString styleSheet1;
    QString styleSheet2;
    if(index == 0){
        //导入样式表-0
        styleSheet1 = getStyleSetting(":/qss/style_1.qss");
        styleSheet2 = getStyleSetting(":/qss/qssStyle_QToolBox.qss");
        qApp->setStyleSheet(styleSheet1+styleSheet2);
    }else if(index == 1){
        //导入样式表-1
        styleSheet1 = getStyleSetting(":/qss/style_2.qss");
        styleSheet2 = getStyleSetting(":/qss/qssStyle_QToolBox.qss");
        qApp->setStyleSheet(styleSheet1+styleSheet2);
    }else if(index == 2){
        //导入样式表-2
        styleSheet1 = getStyleSetting(":/qss/style_3.qss");
        styleSheet2 = getStyleSetting(":/qss/qssStyle_QToolBox.qss");
        qApp->setStyleSheet(styleSheet1+styleSheet2);
    }
}

void Setting::on_ComboBox_2_currentIndexChanged(int index)
{
    QProcess_change_theme(index);
}
//是否选中无线通信模块中的导航位姿和产量监测
void Setting::on_checkBox_4_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        state_IoT_navi = 1;
        // 复选框被选中
        // qDebug() << "复选框已选中";
    } else if (arg1 == Qt::Unchecked) {
        state_IoT_navi = 0;
        // 复选框被取消选中
        // qDebug() << "复选框已取消选中";
    }
}

void Setting::on_checkBox_5_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        state_IoT_harvest = 1;
        // 复选框被选中
        // qDebug() << "复选框已选中";
    } else if (arg1 == Qt::Unchecked) {
        state_IoT_harvest = 0;
        // 复选框被取消选中
        // qDebug() << "复选框已取消选中";
    }
}

void Setting::on_checkBox_7_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        state_IoT_start = 1;
        // 复选框被选中
        // qDebug() << "复选框已选中";
    } else if (arg1 == Qt::Unchecked) {
        state_IoT_start = 0;
        // 复选框被取消选中
        // qDebug() << "复选框已取消选中";
    }
}

void Setting::on_checkBox_8_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        state_Iot_stop = 1;
        // 复选框被选中
        // qDebug() << "复选框已选中";
    } else if (arg1 == Qt::Unchecked) {
        state_Iot_stop = 0;
        // 复选框被取消选中
        // qDebug() << "复选框已取消选中";
    }
}

//点击确认后，车载终端启动定时器，
//再次点击后，车载终端停止定时器。
void Setting::on_pbn_send2zigbee_clicked()
{

    if(!state_IoT_send){
        time_4GCat1->start(ui->lineEdit_TimerSetTime->text().toInt());
        QMessageBox::information(this, "开启多机协同数据共享", "已开启多机协同数据共享");
        state_IoT_send = 1;
//        qDebug() << "1fdsafdsafds";
    }else{
        time_4GCat1->stop();
        state_IoT_send = 0;
        QMessageBox::information(this, "关闭多机协同数据共享", "已关闭多机协同数据共享");
//        qDebug() << "1f232342423423423afds";
    }
}



void Setting::on_pbn_IoT_control_clicked()
{
    QString index;
    QString IoTQString;
    QString cmd;
    if(ui->comboBox_IoT_indexMachine->currentText() == "1"){
        index = "1";
    }else if(ui->comboBox_IoT_indexMachine->currentText() == "2"){
        index = "2";
    }
    if(ui->checkBox_7->isChecked()){
        cmd = "E";
    }else if(ui->checkBox_8->isChecked()){
        cmd = "S";
    }
    IoTQString = QString("@%1,%2").arg(index).arg(cmd);
    SerialSendString_SQ(IoTQString);
    QMessageBox::information(this, "数据发送中", "控制命令已发送！");
}
