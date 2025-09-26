#ifndef SERIALPORT_H
#define SERIALPORT_H


#include <QThread>
#include <QObject>
#include <QSerialPort>
#include <QDebug>
#include <QSerialPortInfo>


class ComSerialPort : public QObject
{
    Q_OBJECT
public:
    //新建对象时就可以新建一个串口对象并独立一个线程运行
    explicit ComSerialPort(QString PortName,qint32 BaudRate,qint32* state,QObject *parent = nullptr);
    ~ComSerialPort();
    bool InitSerial(QString portname,qint32 BaudRate);//初始化串口
    void CloseSerial();//关闭串口
    void getSerialPort();

signals:
    void UpdateData(QByteArray data);//RcvData接收信号后发送出去

public slots:
    void RcvData();//接受最原始的、未经过处理的数据，然后发送信号UpdateData
    void SendSerialData(QByteArray data);//发送数据至串口？

private:
    QSerialPort *Serial;
    QThread* thread;
    int id;
    QByteArray accumulatedData;
};



#endif // SERIALPORT_H
