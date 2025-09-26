//注意要在cmake中find_package(xml)
#ifndef SQQFILE_H
#define SQQFILE_H

#include <QObject>
#include <QDir>
#include <QFileDialog> //弹出fileDialog的界面
#include <QFile>
#include <QDebug>
#include <QDomDocument>//注意要在cmake中find_package(xml)

class sqQFile : public QObject
{
    Q_OBJECT
public:
    explicit sqQFile(QObject *parent = nullptr);
    void openFileDialog(QWidget *parent);
    void openFile(const QString& fPath);
    void modifyXml(const QString& filePath,     //路径名
                   const QString& tagName,      //标签名
                   const QString& tagNameValue, //标签值
                   const QString attributeName, //待修改的子属性名
                   const QString newValue);     //新的子属性值
    QString filePath;
    QStringList fileContent;
signals:

private:


};

#endif // SQQFILE_H
