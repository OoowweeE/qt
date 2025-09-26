#include "serialport.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QProcess>

ComSerialPort::ComSerialPort(QString PortName,qint32 BaudRate,qint32* state, QObject *parent) : QObject(parent)
{

    thread = new QThread();
    Serial = new QSerialPort();

    *state = InitSerial(PortName,BaudRate);

    this->moveToThread(thread);
    Serial->moveToThread(thread);
    thread->start();

    connect(thread, &QThread::finished, this,&QObject::deleteLater);

}

ComSerialPort::~ComSerialPort()
{

}


bool ComSerialPort::InitSerial(QString portname,qint32 BaudRate)
{

    Serial->setPortName(portname);
    if(!Serial->open(QIODevice::ReadWrite))
    {
        qDebug()<<"串口打开失败：" << Serial->errorString();;
        return 0;
    }
    qDebug()<<"串口打开成功";
    Serial->setBaudRate(BaudRate);   // 默认115200
    Serial->setDataBits(QSerialPort::Data8);        // 默认
    Serial->setParity(QSerialPort::NoParity);
    Serial->setStopBits(QSerialPort::OneStop);

    // 读取数据
    connect(Serial,&QSerialPort::readyRead,this,&ComSerialPort::RcvData);

    return 1;
}

void ComSerialPort::CloseSerial()
{

    Serial->clear();
    Serial->close();

}

void ComSerialPort::getSerialPort()
{
    const QList<QSerialPortInfo> serialPorts = QSerialPortInfo::availablePorts();
    if (serialPorts.isEmpty()) {
        qDebug() << "没有检测到可用的串口。";
        return;
    }

    qDebug() << "可用的串口:";
    for (const QSerialPortInfo &portInfo : serialPorts) {
        qDebug() << "串口名称:" << portInfo.portName()
                 << "描述:" << portInfo.description()
                 << "制造商:" << portInfo.manufacturer();
    }
}

void ComSerialPort::RcvData()
{
    QByteArray buffer = Serial->readAll();
    accumulatedData.append(buffer); // 将新接收的数据添加到累积数据中

    // 检查是否包含换行符，假设换行符为 "\r\n"
    if (accumulatedData.endsWith(QString("\r\n").toUtf8())) {
        // 处理完整的数据
        emit UpdateData(accumulatedData);
        accumulatedData.clear(); // 清空缓冲区
    }

    qDebug() << "接收数据线程ID："<< QThread::currentThreadId();
    //emit  UpdateData(buffer);
}

void ComSerialPort::SendSerialData(QByteArray data)
{
    qDebug() << "发送数据线程ID："<< QThread::currentThreadId();
    // 接收GUI数据并发送
    Serial->write(data);
    Serial->flush();
    Serial->waitForBytesWritten(10);
}

