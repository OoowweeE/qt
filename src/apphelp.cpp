#include "apphelp.h"
#include <QIcon>

#include <QDesktopWidget>
#include <QRect>
#include <QPoint>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QDir>
#include <QWebEngineView>   // 引入 QWebEngineView
#include <QWebEngineProfile>
#include <QTimer>
// 构造函数
AppHelp::AppHelp(QWidget *parent): QWidget(parent)
{
    createHelpUI();

    setWindowTitle(tr("使用帮助"));                               // 设置标题
    setWindowIcon(QIcon(":/icon/icon/help.png"));                // 设置图标
    setMinimumSize(800, 400);                                    // 设置大小

    //001:居中显示
    QDesktopWidget desktop;
    QRect rect = desktop.availableGeometry(desktop.primaryScreen());
    QPoint center = rect.center();
    center.setX(center.x() - this->width()/2);
    center.setY(center.y() - this->height()/2);
    move(center);
    //001;

    connect(listWidget, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(listWidgetChanged(QListWidgetItem*)));
}

// 析构函数，确保 QWebEngineView 和相关资源被销毁
AppHelp::~AppHelp() {
    delete webEngineView;  // 延迟删除 QWebEngineView
    //qDebug()<<"appHelp页面关闭";
}

// 创建UI
void AppHelp::createHelpUI()
{
    // 新建全局的水平布局（插入导航栏和其对应的内容）
    QHBoxLayout *mainLayout = new QHBoxLayout;
    setLayout(mainLayout);

    // 新建导航栏及导航栏中的内容
    listWidget = new QListWidget;
    listWidget->addItem(tr("1 基本信息"));
    listWidget->addItem(tr("2 隐私政策"));
//    listWidget->addItem(tr("待定"));
//    listWidget->addItem(tr("待定"));
//    listWidget->addItem(tr("待定"));
//    listWidget->addItem(tr("待定"));
    listWidget->setFixedWidth(100);                            // 修改导航栏的栏宽
    for (int i = 0; i < listWidget->count(); ++i) {            // 遍历:设置每个项的对齐方式为居中
        QListWidgetItem *item = listWidget->item(i);
        item->setTextAlignment(Qt::AlignCenter);               // 设置文本居中对齐
    }
    mainLayout->addWidget(listWidget);                         // 添加导航栏至全局水平布局中

    // 新建内容栏
    QVBoxLayout *rightLayout = new QVBoxLayout;

    // 使用 QWebEngineView 替代 QTextBrowser
    webEngineView = new QWebEngineView;                        // 创建 QWebEngineView 对象
    rightLayout->addWidget(webEngineView);                     // 将 webEngineView 添加到布局中

    mainLayout->addLayout(rightLayout);
    listWidget->setCurrentItem(listWidget->item(0));           // 默认选中导航栏的第一项成员
    listWidgetChanged(listWidget->item(0));                    // 与上面的选中第一项对应，选中第一个内容
}

// 根据当前选中的 QListWidgetItem 来加载不同的 HTML 文件并显示在 QWebEngineView 中
void AppHelp::listWidgetChanged(QListWidgetItem* item) {
    QString htmlFilePath;
    if (listWidget->item(0) == item) {
        htmlFilePath = ":/html/基本信息.html";
    } else if (listWidget->item(1) == item) {
        htmlFilePath = ":/html/隐私政策.html";
    }
//    else if (listWidget->item(2) == item) {
//        htmlFilePath = ":/html/基本信息.html";
//    } else if (listWidget->item(3) == item) {
//        htmlFilePath = ":/html/隐私政策.html";
//    } else if (listWidget->item(4) == item) {
//        htmlFilePath = ":/html/隐私政策.html";
//    } else if (listWidget->item(5) == item) {
//        htmlFilePath = ":/html/Qt-Ros.html";
//    }

    if (!htmlFilePath.isEmpty()) {
        loadHtmlFile(htmlFilePath);
    }
}

void AppHelp::loadHtmlFile(const QString &filePath) {
    QFile txtFile(filePath);
    if (txtFile.open(QIODevice::ReadOnly)) {
        QByteArray htmlData = txtFile.readAll();
        txtFile.close();

        // 使用 QWebEngineView 加载 HTML 内容
        webEngineView->setHtml(htmlData);   // 使用 setHtml() 加载 HTML 内容
    } else {
        // 文件无法打开时的错误处理
        qWarning() << "Unable to open file: " << filePath;
    }
}
