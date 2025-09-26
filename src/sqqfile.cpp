#include "sqqfile.h"

sqQFile::sqQFile(QObject *parent) : QObject(parent)
{

}

void sqQFile::openFileDialog(QWidget *parent)
{
    filePath = QFileDialog::getOpenFileName(parent,"选择文件",QString(),"All Files (*.*)");
    if (!filePath.isEmpty()) {
        qDebug() << "选择的文件夹路径:" << filePath;
    }
}

void sqQFile::openFile(const QString &fPath)
{
    QFile file(fPath);//打开文件

    //如果文件打开失败，输出错误提示
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Cannot open file for reading:" << file.errorString();
        return;
    }
    if (!filePath.isEmpty()) {
        fileContent.clear();
                qDebug() << "选择的文件夹路径:" << filePath;
                QFile file1(filePath);
                if (file1.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    while (!file1.atEnd()) {
                    QByteArray line = file1.readLine();
                    QString lineText = QString::fromUtf8(line).remove(QRegExp("[\r\n]"));//mark!!!这样写可以保留缩进也能避免不必要的空行
                    qDebug() << line.trimmed();
                    fileContent.append(lineText);
                    }
                }else{
                    qDebug() << "无法打开文件";
                }
    }
}

void sqQFile::modifyXml(const QString &filePath, const QString &tagName, const QString &tagNameValue, const QString attributeName, const QString newValue)
{
    QFile file(filePath);//打开文件

    //如果文件打开失败，输出错误提示
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Cannot open file for reading:" << file.errorString();
        return;
    }

    //读取文件内容
    QDomDocument document;
    if(!document.setContent(&file)){
        qDebug() << "Failed to parse the file into a DOM tree.";
        file.close();
        return;
    }
    file.close();//将文件内容解析至document形成DOM树后就可以关闭这个文件了

    //查找特定标签
    QDomNodeList elements = document.elementsByTagName(tagName);//传入参数tagName，指定查找的标签名
    for (int i = 0; i < elements.size(); ++i) {
        QDomElement element = elements.at(i).toElement();
        //相当于两层判断：既要包含包含需要修改的属性名，也要包含这个标签中其他属性和对应的属性名才能改
        if (element.hasAttribute(attributeName) && element.attribute("name") == tagNameValue) {
            // 修改属性值
            element.setAttribute(attributeName, newValue);
        }
    }
    // 将修改内容写入保存到launch文件
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for writing:" << file.errorString();
        return;
    }
    QTextStream out(&file);
    document.save(out, 4); // 4是缩进字符数
    file.close();
}
