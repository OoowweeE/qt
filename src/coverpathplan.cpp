#include "coverpathplan.h"
struct LineEquation {
    double slope;
    double intercept;
};
coverPathPlan::coverPathPlan()
{

}

// points:外部四边形的四个点
// working_wide:割幅
// min_radius：农机最小转弯半径
QList<QPointF> coverPathPlan::getCPP_S(QList<QPointF> points, double working_wide)
{
    double with_co = working_wide;  // 内四边形与外四边形到距离
    double y_plass = 2*working_wide;  // 扫描线的步长
    // 1 执行坐标转换
    auto transformed = this->transform(points);
    QList<QPointF> transformed_points = transformed.first;
    double theta = transformed.second;
//    // 输出转换后的点和旋转角度
//    qDebug() << "转换后的点:";
//    for (const QPointF& point : transformed_points) {
//        qDebug() << "(" << point.x() << "," << point.y() << ")";
//    }
//    qDebug() << "旋转角度 (弧度):" << theta;

    // 2 由点得直线方程序列
    auto line_equations = calculate_line_equations(transformed_points);

    // 3 得内四边形直线方程序列
    QList<LineEquation> small_line_equations = small_quad(line_equations,transformed_points,with_co);

    // 4 扫描线算法：对内直线方程进行相交得点
    double limit3y = transformed_points[2].y();// 第三直线方程的y值
    double limit4y = transformed_points[3].y();// 第四直线方程到y值
    QList<QPointF> scan_transformed_points = move_horizontally(small_line_equations,with_co,y_plass,limit3y,limit4y);

    return scan_transformed_points;
}

// 坐标转换
// 使用：auto transformed = coord.transform(original_points);
//      QList<QPointF> transformed_points = transformed.first;
//      double theta = transformed.second;
std::pair<QList<QPointF>, double> coverPathPlan::transform(const QList<QPointF> &points)
{
    if (points.size() != 4) {
        qWarning() << "需要4个点进行转换";
        //创建并返回一个空的 QList<QPointF> 对象
        //角度为0
        return {QList<QPointF>(), 0.0};
    }

    // 计算第一个点到第二个点的距离（欧式距离，距离的平方=(x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)）
    double distance = std::sqrt(std::pow(points[1].x() - points[0].x(), 2) +
                                std::pow(points[1].y() - points[0].y(), 2));

    // 新坐标系的前两个点
    QList<QPointF> new_points;
    new_points.append(QPointF(0, 0));
    new_points.append(QPointF(distance, 0));

    // 平移所有点，使第一个点位于原点
    QList<QPointF> translated;
    for (const QPointF& point : points) {
        translated.append(QPointF(point.x() - points[0].x(), point.y() - points[0].y()));
    }

    // 计算旋转角度，使第一条边与X轴对齐
    double theta = -std::atan2(translated[1].y(), translated[1].x());

    // 旋转所有点
    QList<QPointF> rotated;
    for (const QPointF& point : translated) {
        double x = point.x() * std::cos(theta) - point.y() * std::sin(theta) + new_points[0].x();
        double y = point.x() * std::sin(theta) + point.y() * std::cos(theta) + new_points[0].y();
        rotated.append(QPointF(x, y));
    }

    return {rotated, theta};
}

QList<QPointF> coverPathPlan::getCPP_O(QList<QPointF> points, double working_wide)
{
    // 参数说明：
    // points: 外部四边形的四个点 (逆时针)
    // working_wide: 农机的工作宽度，也即偏移距离

    double with_co = working_wide;  // 内四边形与外四边形的偏移距离
    QList<QPointF> c; // 最终的覆盖路径点列表

    // 1. 执行坐标系转换，将四边形转换到局部坐标系中，便于计算
    auto transformed = this->transform(points);
    QList<QPointF> transformed_points = transformed.first; // 转换后的四边形顶点
    double theta = transformed.second;                    // 坐标系旋转角度

    // 如果点转换失败，立即返回空列表
    if (transformed_points.size() != 4) {
        qWarning() << "点转换失败：输入的点数不足4个";
        return QList<QPointF>();
    }

    // 输出初始转换结果
    qDebug() << "转换后的点:";
    for (const QPointF& point : transformed_points) {
        qDebug() << "(" << point.x() << "," << point.y() << ")";
    }
    qDebug() << "旋转角度 (弧度):" << theta;

    // 2. 由转换后的点生成直线方程序列
    auto line_equations = calculate_line_equations(transformed_points);

    // 3. 初始化路径规划参数
    QList<QPointF> current_points = transformed_points;   // 当前的四边形顶点
    QList<LineEquation> current_lines = line_equations;   // 当前的直线方程，最初为外部四边形的直线方程
    int iteration_limit = 100;                            // 最大迭代次数，避免死循环
    int iteration_count = 0;                              // 当前迭代次数

    // 4. 开始路径规划迭代
    while (iteration_count < iteration_limit) {
        qDebug() << "迭代次数：" << iteration_count;

        // 4.1 使用 small_quad 函数生成偏移后的四边形直线方程
        QList<LineEquation> equ_i = small_quad(current_lines, current_points, with_co);
        if (equ_i.isEmpty()) {
            qWarning() << "small_quad 返回空，终止规划";
            break;
        }

        // 4.2 计算偏移四边形的交点
        QList<QPointF> b = f_intersection_points(equ_i);
        if (b.size() != 4) {
            qWarning() << "交点数量不足 4，终止规划";
            break;
        }

        // 输出当前偏移四边形的交点
        qDebug() << "当前偏移四边形交点:";
        for (const QPointF& point : b) {
            qDebug() << "(" << point.x() << "," << point.y() << ")";
        }

        // 4.3 将当前偏移四边形的顶点添加到路径点列表中
        // 为了形成连续的 O 型路径，我们需要按顺时针或逆时针顺序添加点
        for(int i = 3;i >= 0;i--){
            c.append(b[i]);
        }
        //c.append(b);

        // 4.4 更新 current_points 和 current_lines 为当前偏移四边形的顶点和线性方程
        current_points = b;
        current_lines = equ_i;

        // 4.5 检查终止条件：
        // 终止条件：内四边形的面积接近零，或偏移距离超过边界
        // 这里我们通过检查内四边形的最小边长来终止
        double min_side_length = std::numeric_limits<double>::max();
        for(int i=0; i < current_points.size(); ++i){
            int j = (i + 1) % current_points.size();
            double distance = std::hypot(current_points[j].x() - current_points[i].x(),
                                         current_points[j].y() - current_points[i].y());
            if(distance < min_side_length){
                min_side_length = distance;
            }
        }

        qDebug() << "当前内四边形最小边长:" << min_side_length;

        if(min_side_length <= with_co){
            qDebug() << "终止条件达成：内四边形最小边长小于或等于工作宽度";
            break;
        }

        // 增加迭代计数器
        iteration_count++;
    }

    // 输出最终路径点（仍然是局部坐标系下的点）
    qDebug() << "最终路径点列表:";
    for (const QPointF& point : c) {
        qDebug() << "(" << point.x() << "," << point.y() << ")";
    }

    // 5. 不进行反坐标转换，直接返回转换后的坐标系中的路径点
    return c;
}
QPointF coverPathPlan::calculate_intersection(const coverPathPlan::LineEquation &l1, const coverPathPlan::LineEquation &l2)
{
    if (std::isinf(l1.slope) && std::isinf(l2.slope)) {
        // 两条垂直线平行，无交点
        return QPointF(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN());
    }

    if (std::isinf(l1.slope)) {
        // l1 是垂直线
        double x = l1.intercept;
        double y = l2.slope * x + l2.intercept;
        return QPointF(x, y);
    }

    if (std::isinf(l2.slope)) {
        // l2 是垂直线
        double x = l2.intercept;
        double y = l1.slope * x + l1.intercept;
        return QPointF(x, y);
    }

    if (qFuzzyCompare(l1.slope, l2.slope)) {
        // 平行线，无交点
        return QPointF(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN());
    }

    // 计算交点
    double x = (l2.intercept - l1.intercept) / (l1.slope - l2.slope);
    double y = l1.slope * x + l1.intercept;
    return QPointF(x, y);
}
QList<QPointF> coverPathPlan::f_intersection_points(const QList<coverPathPlan::LineEquation> &line_equations)
{
    QList<QPointF> intersections;
    for (int i = 0; i < line_equations.size(); ++i) {
        int j = (i + 1) % line_equations.size();
        QPointF intersection = calculate_intersection(line_equations[i], line_equations[j]);
        if (isValidPoint(intersection)) {
            intersections.append(intersection);
        }
    }
    return intersections;
}

bool coverPathPlan::isValidPoint(const QPointF &point)
{
    return !std::isnan(point.x()) && !std::isnan(point.y());
}
QList<QPointF> coverPathPlan::back_transform(const QList<QPointF> &points_origin, const QList<QPointF> &points_transform, double angle)
{
    // 先反旋转（角度已知）
    QList<QPointF> rotated_points;
    for (const QPointF& point : points_transform) {
        double x = point.x() * std::cos(angle) - point.y() * std::sin(angle);
        double y = point.x() * std::sin(angle) + point.y() * std::cos(angle);
        rotated_points.append(QPointF(x, y));
    }
    // 再反平移
    double dx = points_origin[0].x();
    double dy = points_origin[0].y();
    QList<QPointF> final_points;
    for (const QPointF& point : rotated_points) {
        final_points.append(QPointF(point.x() + dx, point.y() + dy));
    }

    return final_points;
}

coverPathPlan::LineEquation coverPathPlan::calculate_slope_intercept(const QPointF &p1, const QPointF &p2) const
{
    LineEquation line;
    //qFuzzyCompare:比较两个数，相同则true，不同则false
    if (qFuzzyCompare(p1.x() + 1, p2.x() + 1)) { // 避免浮点比较问题
        line.slope = std::numeric_limits<double>::infinity(); // 垂直线
        //qDebug() << "这是line.slope" <<line.slope;
        line.intercept = p1.x(); // 对于垂直线，截距表示x值
        qDebug() << "Line (Vertical): x =" << line.intercept;
    } else {
        line.slope = (p2.y() - p1.y()) / (p2.x() - p1.x());
        line.intercept = p1.y() - line.slope * p1.x();
        qDebug() << "Line: y =" << line.slope << "*x +" << line.intercept;
    }
    return line;
}

QList<coverPathPlan::LineEquation> coverPathPlan::calculate_line_equations(const QList<QPointF> &points) const
{
    QList<LineEquation> lines;
    for(int i=0; i < points.size(); ++i){
        // 01，12，23，30
        int j = (i+1) % points.size();
        lines.append(calculate_slope_intercept(points[i], points[j]));
    }
    return lines;
}

double coverPathPlan::calculate_x_coordinate(double slope, double intercept, double known_y) const
{
    if (std::isinf(slope)) {
        qDebug() << "Vertical line: Returning x-intercept =" << intercept;
        return intercept;  // 垂直线，直接返回 x 截距
    }
    double x = (known_y - intercept) / slope;
    qDebug() << "Calculated x =" << x << "for y =" << known_y << "on line with slope =" << slope << "and intercept =" << intercept;
    return x;
}

double coverPathPlan::calculate_y_coordinate(double slope, double intercept, double known_x) const
{
    return slope * known_x + intercept;
}

QPointF coverPathPlan::calculate_centroid(const QList<QPointF> &points) const
{
    double x=0.0, y=0.0;
    for(const QPointF& p : points){
        x += p.x();
        y += p.y();
    }
    QPointF centroid(x / points.size(), y / points.size());
    qDebug() << "Centroid:" << centroid;
    return centroid;
}

QList<coverPathPlan::LineEquation> coverPathPlan::small_quad(const QList<coverPathPlan::LineEquation> &lines, const QList<QPointF> &points, double with_co) const
{
    if (lines.size() != 4 || points.size() !=4 ){
        qWarning() << "small_quad requires exactly 4 lines and 4 points.";
        return QList<LineEquation>();
    }

    QPointF centroid = calculate_centroid(points);
    QList<LineEquation> offset_lines = lines;

    // Offset Line 0 (Bottom Edge): Directly upward
    offset_lines[0].intercept += with_co;
    qDebug() << "Offset Line 0: y =" << offset_lines[0].slope << "*x +" << offset_lines[0].intercept;

    // Offset Line 1 (Right Edge)
    LineEquation line1 = lines[1];
    if(std::isinf(line1.slope)){
        // Vertical line
        if(centroid.x() > line1.intercept){
            // Shift left
            offset_lines[1].intercept += with_co;
            qDebug() << "Offset Line 1 (Vertical, Shift Left): x =" << offset_lines[1].intercept;
        }
        else{
            // Shift right
            offset_lines[1].intercept -= with_co;
            qDebug() << "Offset Line 1 (Vertical, Shift Right): x =" << offset_lines[1].intercept;
        }
    }
    else{
        // Non-vertical line
        double angle = std::atan(line1.slope);
        double delta = with_co / std::cos(angle);
        if(line1.slope < 0){
            // Shift downward
            offset_lines[1].intercept -= delta;
            qDebug() << "Offset Line 1 (Slope < 0, Shift Down): y =" << line1.slope << "*x +" << offset_lines[1].intercept;
        }
        else{
            // Shift upward
            offset_lines[1].intercept += delta;
            qDebug() << "Offset Line 1 (Slope > 0, Shift Up): y =" << line1.slope << "*x +" << offset_lines[1].intercept;
        }
    }

    // Offset Line 2 (Top Edge)
    LineEquation line2 = lines[2];
    if(!std::isinf(line2.slope)){
        double angle = std::atan(line2.slope);
        double delta = with_co / std::cos(angle);
        // Always shift downward
        offset_lines[2].intercept -= delta;
        qDebug() << "Offset Line 2 (Top Edge, Always Shift Down): y =" << line2.slope << "*x +" << offset_lines[2].intercept;
    }

    // Offset Line 3 (Left Edge)
    LineEquation line3 = lines[3];
    if(std::isinf(line3.slope)){
        // Vertical line
        if(centroid.x() > line3.intercept){
            // Shift left
            offset_lines[3].intercept += with_co;
            qDebug() << "Offset Line 3 (Vertical, Shift Left): x =" << offset_lines[3].intercept;
        }
        else{
            // Shift right
            offset_lines[3].intercept -= with_co;
            qDebug() << "Offset Line 3 (Vertical, Shift Right): x =" << offset_lines[3].intercept;
        }
    }
    else{
        // Non-vertical line
        double angle = std::atan(line3.slope);
        double delta = with_co / std::cos(angle);
        if(line3.slope < 0){
            // Shift upward
            offset_lines[3].intercept += delta;
            qDebug() << "Offset Line 3 (Slope < 0, Shift Up): y =" << line3.slope << "*x +" << offset_lines[3].intercept;
        }
        else{
            // Shift downward
            offset_lines[3].intercept -= delta;
            qDebug() << "Offset Line 3 (Slope > 0, Shift Down): y =" << line3.slope << "*x +" << offset_lines[3].intercept;
        }
    }

    qDebug() << "Offset Line Equations:";
    for(int i=0; i<4; ++i){
        if(std::isinf(offset_lines[i].slope)){
            qDebug() << "Offset Line" << i << ": x =" << offset_lines[i].intercept;
        }
        else{
            qDebug() << "Offset Line" << i << ": y =" << offset_lines[i].slope << "*x +" << offset_lines[i].intercept;
        }
    }

    return offset_lines;
}

QList<QPointF> coverPathPlan::move_horizontally(const QList<coverPathPlan::LineEquation> &line_equations, double with_co, double y_plass, double limit3, double limit4)
{
    QList<QPointF> intersection_points;  // 用于存储交点
    LineEquation l2 = line_equations[1];
    LineEquation l3 = line_equations[2];
    LineEquation l4 = line_equations[3];

    if (limit3 < limit4) {
        while (y_plass < limit4) {
            qDebug() << "Processing scanline at y =" << y_plass;

            // 计算与第四条直线 (l4) 的交点
            double x_4 = calculate_x_coordinate(l4.slope, l4.intercept, y_plass);
            qDebug() << "Intersection with l4 at (x_4, y_plass) = (" << x_4 << ", " << y_plass << ")";

            if (y_plass < limit3) {
                // 计算与第二条直线 (l2) 的交点
                double x_2 = calculate_x_coordinate(l2.slope, l2.intercept, y_plass);
                qDebug() << "Intersection with l2 at (x_2, y_plass) = (" << x_2 << ", " << y_plass << ")";
                intersection_points.append(QPointF(x_2, y_plass));

            } else {
                // 计算与第三条直线 (l3) 的交点
                double x_3 = calculate_x_coordinate(l3.slope, l3.intercept, y_plass);
                qDebug() << "Intersection with l3 at (x_3, y_plass) = (" << x_3 << ", " << y_plass << ")";
                if (x_3 >= x_4) {
                    intersection_points.append(QPointF(x_3, y_plass));
                } else {
                    intersection_points.append(QPointF(x_4, y_plass));
                    //intersection_points.append(QPointF(x_2, y_plass));
                    break;
                }
            }

            intersection_points.append(QPointF(x_4, y_plass));
            //y_plass += with_co;
            y_plass += 2*with_co;
        }
    } else {
        while (y_plass < limit3) {
            qDebug() << "Processing scanline at y =" << y_plass;

            // 计算与第二条直线 (l4) 的交点
            double x_2 = calculate_x_coordinate(l2.slope, l2.intercept, y_plass);
            qDebug() << "Intersection with l4 at (x_4, y_plass) = (" << x_2 << ", " << y_plass << ")";

            if (y_plass < limit4) {
                double x_4 = calculate_x_coordinate(l4.slope, l4.intercept, y_plass);
                intersection_points.append(QPointF(x_4, y_plass));
            } else {
                // 计算与第三条直线 (l3) 的交点
                double x_3 = calculate_x_coordinate(l3.slope, l3.intercept, y_plass);
                qDebug() << "Intersection with l3 at (x_3, y_plass) = (" << x_3 << ", " << y_plass << ")";
                if (x_3 <= x_2) {
                    intersection_points.append(QPointF(x_3, y_plass));
                } else {
                    // 计算与第二条直线 (l2) 的交点
                    double x_2 = calculate_x_coordinate(l2.slope, l2.intercept, y_plass);
                    qDebug() << "Intersection with l2 at (x_2, y_plass) = (" << x_2 << ", " << y_plass << ")";
                    intersection_points.append(QPointF(x_2, y_plass));
                    break;
                }
            }

            intersection_points.append(QPointF(x_2, y_plass));
            y_plass += 2*with_co;
            //y_plass += y_plass;
        }
    }

    // {A, B, C, D, E, F, G, H}->{A, B, D, C, E, F, H, G}
    QList<QPointF> tranPoints = swapPoints(intersection_points);
    // 输出所有交点
    qDebug() << "Final intersection points:";
    for (const QPointF& point : intersection_points) {
        qDebug() << point;
    }
    return intersection_points;
}

// {A, B, C, D, E, F, G, H}->{A, B, D, C, E, F, H, G}
QList<QPointF> coverPathPlan::swapPoints(QList<QPointF> &points)
{
    int size = points.size();
    // 遍历点集，每隔两个点进行交换
    for (int i = 2; i < size; i += 4) {
        if (i + 1 < size) {
            QPointF temp = points[i];
            points[i] = points[i + 1];
            points[i + 1] = temp;
        }
    }
    return points;
}
