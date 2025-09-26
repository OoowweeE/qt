#include "terminal.h"
#include "ui_terminal.h"
#include <QDebug>
#include "globalsignal.h"
#include <QMainWindow>
Terminal::Terminal(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Terminal)
{
    ui->setupUi(this);
    moveToCenter();
    ui->TextEdit_In->setFocus();//将初始坐标定位到输入框中
    process = new QProcess;//创建一个process对象，用于模拟terminal
//判断当前操作系统
#ifdef Q_OS_WIN
    process->start("cmd.exe");
#elif defined(Q_OS_LINUX)
    process->start("bash");
#endif
    //0-链接点击按钮响应槽函数
    connect(ui->Pbt_Send,SIGNAL(clicked()),this,SLOT(slot_Pbt_Send()));
    //1-报错日志数据
    connect(process,SIGNAL(readyReadStandardError()),this,SLOT(slot_process_Output()));
    //2-正常日志数据
    connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(slot_process_Output()));

    connect(GlobalSignal::instance(), &GlobalSignal::globalSignal_KeyPress,this, &Terminal::slot_Pbt_Send);
}

Terminal::~Terminal()
{
    delete ui;
}

//将窗口居中显示
void Terminal::moveToCenter()
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

void Terminal::AddButton()
{
//    QPushButton *newButton = new QPushButton(QString("Button %1").arg(++buttonCount));
//    ui->scrollArea_2->layout()->addWidget(newButton);
//    qDebug() << ui->scrollArea_2->widget();
}

void Terminal::slot_Pbt_Send()
{
    //qDebug() << "按键";
//    process->start("bash");
    //每次执行前需要source一下
    QString stringSource = "source ~/.bashrc && ";
    QString cmdTest = "source /home/nvidia/ws/devel/setup.bash";

    //向终端写入命令
    process->write(stringSource.toLocal8Bit()+ui->TextEdit_In->toPlainText().toLocal8Bit()+'\n');
    process->write(cmdTest.toLocal8Bit()+'\n');
    //process->write( terminalCommand+'\n');
    ui->TextEdit_In->clear();
    //链接process回显信号与槽
//    //1-报错日志数据
//    connect(process,SIGNAL(readyReadStandardError()),this,SLOT(slot_process_Output()));
//    //2-正常日志数据
//    connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(slot_process_Output()));



}


void Terminal::slot_process_Output()
{
    QByteArray output = process->readAllStandardOutput();
    QByteArray error = process->readAllStandardError();

    // 处理标准输出
    if (!output.isEmpty()) {
        QString outputText = QString::fromLocal8Bit(output).trimmed();
        if (!outputText.isEmpty()) {
            ui->TextEdit_Out->append(outputText);
        }
    }

    // 处理标准错误
    if (!error.isEmpty()) {
        QString errorText = QString::fromLocal8Bit(error).trimmed();
        if (!errorText.isEmpty()) {
            ui->TextEdit_Out->append(errorText);
        }
    }
}

//重载按键绑定发送信号
void Terminal::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_F1) {
        // 假设有一个按钮 ui->yourButton
        ui->Pbt_Send->click();  // 模拟按钮被点击
        //qDebug() << "按键event";
    } else {
        QWidget::keyPressEvent(event);  // 处理其他按键事件
    }
}


void Terminal::on_pbt_ctrl_C_clicked()
{
    //杀死进程
    process->write(QString(QChar(0x03)).toUtf8());
    process->kill();
    process->close();
    //重启终端
    process->start("bash");
}

void Terminal::on_pbt_clc_clicked()
{
    ui->TextEdit_Out->clear();
}




void Terminal::on_pbt_HistoryShow_clicked()
{
    if(HistoryShowState == 0){
        ui->TextEdit_Out->setVisible(false);
        HistoryShowState = 1;
    }
    else{
        ui->TextEdit_Out->setVisible(true);
        HistoryShowState = 0;
    }
}

void Terminal::on_pbt_saveCMD_clicked()
{
    AddButton();
}
