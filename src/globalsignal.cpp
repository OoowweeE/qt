#include "globalsignal.h"

GlobalSignal::GlobalSignal(QObject *parent) : QObject(parent)
{

}

GlobalSignal* GlobalSignal::instance() {
    static GlobalSignal instance;
    return &instance;
}

QList<QPointF> GlobalSignal::ReadCSV_XY(const QString &filePath)
{
    QList<QPointF> tmpXY;  // 用于存储整合后的 QPointF

    // 读取 CSV 文件的第0列（x）和第1列（y）
    rapidcsv::Document doc(filePath.toStdString(),rapidcsv::LabelParams(-1,-1));
    std::vector<float> x_1 = doc.GetColumn<float>(0);
    std::vector<float> y_1 = doc.GetColumn<float>(1);

    // 打印 x_1 和 y_1 的大小
    std::cout << "Read " << x_1.size() << " x values." << std::endl;
    std::cout << "Read " << y_1.size() << " y values." << std::endl;
    // 检查 x_1 和 y_1 是否大小一致
    if (x_1.size() != y_1.size()) {//如果不一致，输出报错
        std::cerr << "Error: x and y columns have different sizes!" << std::endl;
        return tmpXY;
    }
    // 将 x_1 和 y_1 中的值整合为 QPointF，并存入 QList
    for (size_t i = 0; i < x_1.size(); ++i) {
        QPointF point(x_1[i], y_1[i]);  // 创建 QPointF
        tmpXY.append(point);            // 添加到 QList
        // 打印每个 QPointF 的值
        std::cout << "QPointF(" << point.x() << ", " << point.y() << ")" << std::endl;
    }
    return tmpXY;
}

namespace globalValue {
    int globalVariable = 0;//test
    QList<QPointF> readCsvPath;
    bool flag_readPath = 1;
    bool flag_launch_WayPointLoader = 1;
    bool flag_launch_autoNavigation = 1;//是否开启路径跟踪
    bool flag_launch_savePath = 1;
    ANetData_t ANetData = {"ANet01", 0, 0, 0.0, 'N', 0.0, 'E', 0.0, 'm',0.0,'s'}; //创建ros存储对象
    canData_t canData = {0,"",0.0,'N',0.0,'E',0.0,'m',0.0,0.0,0.0,"m/s"};
    const char* buttonStyle_UnChecked = R"(
        QPushButton{
            color:#ffffff;
            background-color:#63A103;
            border: none;
            border-top-left-radius:10px;
            border-bottom-left-radius:10px;
        }
    )";//按键被按下时的状态
    const char* buttonStyle_Checked = R"(
        QPushButton{
              color:#ffffff;
              background-color:#428752;
              border: none;
              border-top-left-radius:10px;
              border-bottom-left-radius:10px;
        }
    )";//按键被按下时的状态
}



