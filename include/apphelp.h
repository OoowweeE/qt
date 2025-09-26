#ifndef APPHELP_H
#define APPHELP_H

#include <QWidget>
#include <QPushButton>
#include <QTextBrowser>
#include <QStackedWidget>
#include <QListWidget>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEnginePage>

class AppHelp : public QWidget
{
    Q_OBJECT
public:
    //构造函数
    explicit AppHelp(QWidget *parent = 0);
    ~AppHelp();
    //创建UI
    void createHelpUI();

    void loadHtmlFile(const QString &filePath);
private slots:
    //当前列表项变化
    void listWidgetChanged(QListWidgetItem*);

private:
    QListWidget *listWidget;
    QTextBrowser *textBrowser;     // 未使用
    QWebEngineView *webEngineView; // 用 QWebEngineView 替换 QTextBrowser
};

#endif // APPHELP_H
