#include "work_obstacle.h"
#include "ui_work_obstacle.h"

//这边是不是应该需要弄个信号和槽，当Rosinit后，再执行草函数，让Obstacle页面初始化
//未初始化前页面应该是灰色的（表示不能被操作）
work_Obstacle::work_Obstacle(int argc,char** argv,QWidget *parent,QNode *RosNode) :
    QWidget(parent),
    ui(new Ui::work_Obstacle)
{
    ui->setupUi(this);
    rvizNode = RosNode;

    //qDebug() << "0520-1-4-1";
    //mark:需要传入标志量（改Ros连接状态）
    if(rvizNode->init()){
        //qDebug() << "0520-1-4-1-1";
        myrviz = new qrviz(ui->RVizLayout); //mark
        QMessageBox::information(nullptr, "欢迎使用农机终端", "Ros已连接");
    }else{
        QMessageBox::information(nullptr, "欢迎使用农机终端", "Ros未连接，请前往设置页面重新初始化");
    }
    //绑定图像话题
    connect(rvizNode,SIGNAL(image_val(QImage)),this,SLOT(slot_update_image(QImage)));
//    connect(,this,SLOT(slot_sub_image()));
    //myrviz = new qrviz(ui->RVizLayout); //mark
    //qDebug() << "0520-1-4-2";
    #pragma region rViz 树组件设计代码
    //设置树组件头
    //qDebug() << "0520-1-4-2-1";
    ui->treeWidget_layer->setHeaderLabels(QStringList() << "key" << "value");
    //设置拉伸最后列属性为false
    ui->treeWidget_layer->header()->setStretchLastSection(false);
    //设置所有列自动拉伸
    ui->treeWidget_layer->header()->setSectionResizeMode(QHeaderView::Stretch);
    //连接信号和槽：当树形组件发生变化后，执行槽函数
    //slot_display_layer_description：更新选中控件的描述
    //qDebug() << "0520-1-4-2-2";
    connect(ui->treeWidget_layer, &QTreeWidget::currentItemChanged, this, &work_Obstacle::slot_display_layer_description);
    //slot_enable_remove_button：让移除控件可用
    connect(ui->treeWidget_layer, &QTreeWidget::currentItemChanged, this, &work_Obstacle::slot_enable_remove_button);
    qDebug() << "0520-1-4-2-3";
    /***********************************************************************************************
     *                                     设计功能选择区                                     *
     ***********************************************************************************************/
    Function = new QTreeWidgetItem(QStringList() << "模式选择");
    Function->setIcon(0,QIcon("://rViz/options.png"));
    QComboBox *functionBox = new QComboBox();
    functionBox->addItems(QStringList() << "视觉" << "LiDAR" << "多传感器融合");
    functionBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);  //设置尺寸调整策略为适应最小内容
    functionBox->setEditable(true);                                              //设置可编辑
    functionBox->setCurrentIndex(-1);
    //连接combobox当前文本更改信号与全局设置槽函数
    connect(functionBox, SIGNAL(currentIndexChanged(int)),this, SLOT(slot_set_function_options(int)));

    //加载根节点
    ui->treeWidget_layer->addTopLevelItem(Function);
    ui->treeWidget_layer->setItemWidget(Function,1,functionBox);
    //Function->setExpanded(true);
    /***********************************************************************************************
     *                                     设计Global Options树                                     *
     ***********************************************************************************************/
    Global = new QTreeWidgetItem(QStringList() << "全局设置");
    //设置图标
    Global->setIcon(0,QIcon("://rViz/options.png"));
    //加载根节点
    ui->treeWidget_layer->addTopLevelItem(Global);
    //设置默认展开
    Global->setExpanded(true);

    /*
     * 定义子节点1 ———— Fixed Frame(FixedFrame是RViz中用来表示“世界”坐标系的参考系。FixedFrame应该是相对于世界不动的，否则会导致显示错误)
     * 所有的固定数据应该转换到固定坐标系下。比如一堵墙的坐标应该是个固定数据吧。但如果我们把FixedFrame设置为“base_link”(机器人本体坐标系)的话，
     * 那么该坐标系会随着机器人运动而运动，那墙的坐标在该坐标系下也是运动的，这是不应该的。所以一般也不会把FixedFrame设置为“base_link”。
     * 而“map”是一个虚拟的世界坐标系，它的z轴指向正上方(天空)，它就是一个固定的坐标系，不会随着机器人运动而变化。它通常是其他坐标系的父节点，例如
     * “odom”或“base_link”。它可以通过gmapping或amcl等定位模块计算并发布。
     * “odom”是一个局部参考系，它通常是“map”的子坐标系、“base_link”的父坐标系。odom是由运动源，例如轮子运动，视觉偏移等，来计算出来的。odom相
     * 对于机器人的位置是不变的，但是相对于世界的位置是随时间变化的。odom的准确性使它在局部参考系中很有用，但是不适合作为全局参考系，因为它会有累积
     * 误差。如果我们只关心机器人的短期运动状态，也可以把FixedFrame设置为“odom”。
     */
    QTreeWidgetItem* FixedFrame = new QTreeWidgetItem(QStringList() << "固定坐标系");
    //初始化一个combobox
    FFCBox = new QComboBox();
    FFCBox->addItems(QStringList() << "map" << "odom" << "velodyne");                     //添加项
    FFCBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);  //设置尺寸调整策略为适应最小内容
    FFCBox->setEditable(true);                                              //设置可编辑
    //连接combobox当前文本更改信号与全局设置槽函数
    connect(FFCBox, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_set_global_options);
    //添加子节点1
    Global->addChild(FixedFrame);
    //添加combobox于子节点1的第一列
    ui->treeWidget_layer->setItemWidget(FixedFrame, 1, FFCBox);

    /*
     * 定义子节点2 ———— Background Color
     */
    QTreeWidgetItem* BackgroundColor = new QTreeWidgetItem(QStringList() << "背景颜色");
    //添加子节点2
    Global->addChild(BackgroundColor);
    //初始化一个MyPalette对象
    BPalette = new MyPalette();
    //设置初始颜色
    BPalette->setColor(46, 52, 54);
    //添加mypalette于子节点2的第一列
    ui->treeWidget_layer->setItemWidget(BackgroundColor, 1, BPalette);
    //连接mypalette颜色改变信号与全局设置槽函数
    connect(BPalette, &MyPalette::colorchanged, this, &work_Obstacle::slot_set_global_options);

    /*
     * 定义子节点3 ———— image
     */
    QTreeWidgetItem *imageItem = new QTreeWidgetItem(QStringList() << "图像");
    Global->addChild(imageItem);
    //imageItem->setIcon(0,QIcon("://rViz/image.png"));
    imageItemBox = new QComboBox();
    imageItemBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);  //设置尺寸调整策略为适应最小内容
    imageItemBox->addItems(QStringList() << "/usb_cam/image_raw" );
    imageItemBox->setEditable(true);                                              //设置可编辑
    imageItemBox->setCurrentIndex(-1);

    ui->treeWidget_layer->setItemWidget(imageItem, 1, imageItemBox);
    //连接combobox当前文本更改信号与全局设置槽函数
    connect(imageItemBox, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_sub_image);
    //connect(imageItemBox, SIGNAL(currentTextChanged()),this, SLOT(slot_sub_image()));

    //qDebug() << "0520-1-4-2-4";
    get_common_rviz_Display();//把肖师兄需要到控件导入进来
    //qDebug() << "0520-1-4-3";
}

work_Obstacle::~work_Obstacle()
{
    delete ui;
}

void work_Obstacle::removeChildren(QTreeWidgetItem *item)
{
    for (int i = item->childCount() - 1; i >= 0; i--) {
      if(item->child(i)->childCount() > 0)
      {
        removeChildren(item->child(i)); //递归删除子节点的子节点
      }
      //因为QTreeWidgetItem::takeChild(int index)方法会从UI界面移除index下标的项并返回它，所以我们直接delete其返回值
      delete item->takeChild(i);        //从UI界面移除项并释放其内存
    }
}

/**
 * @brief 设置全局设置槽函数
 */
void work_Obstacle::slot_set_global_options()
{
  myrviz->SetFixedFrame(FFCBox->currentText());                //设置固定坐标系
  myrviz->SetPanelBackgroundColor(BPalette->currentColor());   //设置背景颜色
}

void work_Obstacle::slot_set_function_options(int index)
{
    if(index == 0){
//        QString launchName = "test";
//        QString process_launchName = "roslaunch " + launchName + " " +launchName+".launch";
//        qDebug() << "执行命令：" << process_launchName;//打印输出，检查状态
        QMessageBox::information(nullptr, "模式提示", "模式1已启动");
    }else if(index == 1){

        QMessageBox::information(nullptr, "模式提示", "模式2已启动");
    }else if(index == 2){

        QMessageBox::information(nullptr, "模式提示", "模式3已启动");
    }

}


/**
 * @brief 显示图层描述槽函数
 */
void work_Obstacle::slot_display_layer_description()
{
  if(ui->treeWidget_layer->currentItem() == Global){
    ui->textBrowser_layerDescription->setText("全局设置");
  }
  else if(ui->treeWidget_layer->currentItem()->parent() == nullptr){
    switch(ui->treeWidget_layer->currentItem()->data(0, Qt::UserRole).value<int>())
    {
      case AddLayer::grid:
        ui->textBrowser_layerDescription->setText("网格\n\n\t沿地平面显示以目标参考系原点为中心的网格。");
      break;
      case AddLayer::tf:
        ui->textBrowser_layerDescription->setText("坐标变换\n\n\t显示坐标转换层次结构。");
      break;
      case AddLayer::laserscan:
        ui->textBrowser_layerDescription->setText("激光扫描\n\n\t将来自sensor_msgs::LaserScan消息的数据显示为世界中的点，以点、公告板或立方体的形式绘制。");
      break;
      case AddLayer::pointcloud2:
        ui->textBrowser_layerDescription->setText("点云2\n\n\t将来自sensor_msgs::PointCloud2消息的数据显示为世界中的点，以点、公告板或立方体的形式绘制。");
      break;
      case AddLayer::robotmodel:
        ui->textBrowser_layerDescription->setText("机器人模型\n\n\t以正确的姿态(由当前坐标变换定义)显示机器人的视觉表示。");
      break;
      case AddLayer::map:
        ui->textBrowser_layerDescription->setText("地图\n\n\t将来自nav_msgs::OccupancyGrid消息的数据显示为一个地平面的占用网格。");
      break;
      case AddLayer::path:
        ui->textBrowser_layerDescription->setText("路径\n\n\t以线的形式显示nav_msgs::Path消息中的数据。");
      break;
      case AddLayer::pointstamped:
        ui->textBrowser_layerDescription->setText("带点戳\n\n\t显示来自geometry_msgs/PointStamped消息的信息。");
      break;
      case AddLayer::marker:
        ui->textBrowser_layerDescription->setText("标记\n\n\t显示visualization_msgs::Marker消息。");
      break;
      case AddLayer::markerarray:
        ui->textBrowser_layerDescription->setText("标记数组\n\n\t在不假定话题名称以“_array”结尾的情况下显示visualization_msgs::MarkerArray消息。");
      break;
      case AddLayer::image:
        ui->textBrowser_layerDescription->setText("图像\n\n\tDisplays an image from a sensor_msgs/Image topic, similar to image_view.");
      break;
      case AddLayer::pose:
        ui->textBrowser_layerDescription->setText("Pose\n\n\tDisplays a geometry_msgs::PoseStamped message.");
      break;
      case AddLayer::boundingboxarray:
        ui->textBrowser_layerDescription->setText("BoundingBoxArray\n\n\tvisualize jsk_recognition_msgs/BoundingBoxArray ");
      break;
    }
  }
  else { ui->textBrowser_layerDescription->clear(); }
}



/**
 * @brief 设置rviz树item激活槽函数
 */
void work_Obstacle::slot_enable_remove_button(QTreeWidgetItem* current)
{
  //current->parent() == nullptr：检查 current 是否为 顶层节点（没有父节点）
  //current != Global ：检查current 不是 Global才可以被删除（不能把全部都给删了）
  if(current->parent() == nullptr && current != Global)
    ui->button_remove_layer->setEnabled(true);
  else
      ui->button_remove_layer->setEnabled(false);
}
//让label显示图像信息
void work_Obstacle::slot_update_image(QImage im)
{
    // 获取标签的可用大小
    QSize labelSize = ui->label_image->size();

    // 按比例缩放图像
    QPixmap pixmap = QPixmap::fromImage(im).scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 设置图像
    ui->label_image->setPixmap(pixmap);

    // 可选：如果希望标签自动调整大小以适应图像
    ui->label_image->adjustSize();
    //ui->label_image->setPixmap(QPixmap::fromImage(im));
}

//选择图像话题
void work_Obstacle::slot_sub_image()
{
    rvizNode->sub_image(imageItemBox->currentText());
    qDebug() << imageItemBox->currentText();
}


/**
 * @brief 添加图层按钮点击槽函数
 */
void work_Obstacle::on_button_add_layer_clicked()
{
  //打开新增图层对话框并获得其返回值
  int addLayerCode = AddLayer::getLayer(this);
  //获取图层显示名
  QString displayName = AddLayer::getDisplayName();
  //获取话题名
  QString topicName = AddLayer::getTopicName();
  switch(addLayerCode){
    case AddLayer::null:
    break;
    case AddLayer::grid:
      add_TreeWidgetItem_Grid(ui->treeWidget_layer, displayName);
    break;
    case AddLayer::tf:
      add_TreeWidgetItem_TF(ui->treeWidget_layer, displayName);
    break;
    case AddLayer::laserscan:
      add_TreeWidgetItem_LaserScan(ui->treeWidget_layer, displayName, topicName);
    break;
    case AddLayer::pointcloud2:
      add_TreeWidgetItem_PointCloud2(ui->treeWidget_layer, displayName, topicName);
    break;
    case AddLayer::robotmodel:
      add_TreeWidgetItem_RobotModel(ui->treeWidget_layer, displayName);
    break;
    case AddLayer::map:
      add_TreeWidgetItem_Map(ui->treeWidget_layer, displayName, topicName);
    break;
    case AddLayer::path:
      add_TreeWidgetItem_Path(ui->treeWidget_layer, displayName, topicName);
    break;
    case AddLayer::pointstamped:
      add_TreeWidgetItem_PointStamped(ui->treeWidget_layer, displayName, topicName);
    break;
    case AddLayer::marker:
      add_TreeWidgetItem_Marker(ui->treeWidget_layer, displayName, topicName);
    break;
    case AddLayer::markerarray:
      add_TreeWidgetItem_MarkerArray(ui->treeWidget_layer, displayName, topicName);
    break;
    case AddLayer::image:
      add_TreeWidgetItem_Image(ui->treeWidget_layer, displayName, topicName);
    break;
    case AddLayer::pose:
      add_TreeWidgetItem_Pose(ui->treeWidget_layer, displayName, topicName);
    break;
    case AddLayer::boundingboxarray:
      add_TreeWidgetItem_BoundingBoxArray(ui->treeWidget_layer, displayName, topicName);//注意修改函数后缀
    break;
  }
}

/***********************************************************************************************
 *                                向UI界面添加新图层相关控件方法定义                                 *
 ***********************************************************************************************/
/**
 * @brief 向QTreeidget添加Grid顶层节点
 * @param QTreeWidget* treeWidget 待添加Grid顶层节点的QTreeWidget指针
 */
QTreeWidgetItem* work_Obstacle::add_TreeWidgetItem_Grid(QTreeWidget* treeWidget, QString& displayName)
{
  QTreeWidgetItem* Grid = new QTreeWidgetItem(QStringList() << displayName);
  //设置图标
  Grid->setIcon(0, QIcon("://rViz/Grid.png"));
  //初始化一个checkbox
  QCheckBox* checkbox = new QCheckBox();
  //设置复选框默认选中
  checkbox->setCheckState(Qt::Checked);
  //加载根节点
  treeWidget->addTopLevelItem(Grid);
  //设置Grid树默认展开(默认展开应写在加载根节点后)
  Grid->setExpanded(true);
  //添加checkbox于根节点第一列
  treeWidget->setItemWidget(Grid, 1, checkbox);
  //连接checkbox状态改变信号与修改网格图层属性槽函数
  connect(checkbox, &QCheckBox::stateChanged, this, &work_Obstacle::slot_modify_grid_attribute);
  //设置用户数据
  Grid->setData(0, Qt::UserRole, AddLayer::grid);

  /*
   * 定义子节点1 ———— plane cell count
   */
  QTreeWidgetItem* PlaneCellCount = new QTreeWidgetItem(QStringList() << "平面单元格数");
  //添加子节点1
  Grid->addChild(PlaneCellCount);
  //初始化一个spinbox
  QSpinBox* spinbox = new QSpinBox();
  spinbox->setMinimumWidth(150);                              //设置最小宽度
  spinbox->setRange(1,100);                                   //设置可调节范围
  spinbox->setValue(10);                                      //设置初始值
  //把spinbox添加到子节点1的第一列
  treeWidget->setItemWidget(PlaneCellCount, 1, spinbox);
  //定义函数指针(因为QSpinBox::valueChanged存在重载，不使用函数指针会产生调用不明确的错误)
  void (QSpinBox:: *signalpointer1)(int) = &QSpinBox::valueChanged;
  //连接spinbox的值改变信号与修改网格图层属性槽函数
  connect(spinbox, signalpointer1, this, &work_Obstacle::slot_modify_grid_attribute);

  /*
   * 定义子节点2 ———— color
   */
  QTreeWidgetItem* GridColor = new QTreeWidgetItem(QStringList() << "颜色");
  //添加子节点2
  Grid->addChild(GridColor);
  //初始化一个MyPalette对象
  MyPalette* palette = new MyPalette();
  //设置初始颜色
  palette->setColor(160, 160, 164);
  //把mypalette添加到子节点2的第一列
  treeWidget->setItemWidget(GridColor, 1, palette);
  //连接mypalette颜色改变信号与修改网格图层属性槽函数
  connect(palette, &MyPalette::colorchanged, this, &work_Obstacle::slot_modify_grid_attribute);

  //调用qrviz类的接口创建图层对象实体并取得其指针
  rviz::Display* display = myrviz->CreateDisplay("rviz/Grid", "myGrid");
  //向Hash表中插入键值对
  m_LayerHash.insert(Grid, display);
  //默认调用一次修改图层属性函数
  myrviz->ModifyGridAttribute(display, spinbox->value(), palette->currentColor(), checkbox->isChecked());

  return Grid;
}



/**
 * @brief 向QTreeidget添加TF顶层节点
 * @param QTreeWidget* treeWidget 待添加TF顶层节点的QTreeWidget指针
 */
QTreeWidgetItem* work_Obstacle::add_TreeWidgetItem_TF(QTreeWidget* treeWidget, QString& displayName)
{
  QTreeWidgetItem* TF = new QTreeWidgetItem(QStringList() << displayName);
  //设置图标
  TF->setIcon(0,QIcon("://rViz/TF.png"));
  //初始化一个checkbox
  QCheckBox* checkbox = new QCheckBox();
  //设置复选框默认选中
  checkbox->setCheckState(Qt::Checked);
  //连接checkbox状态改变信号与修改TF图层属性槽函数
  connect(checkbox, &QCheckBox::stateChanged, this, &work_Obstacle::slot_modify_TF_attribute);
  //加载根节点
  treeWidget->addTopLevelItem(TF);
  //添加checkbox于根节点第一列
  treeWidget->setItemWidget(TF, 1, checkbox);
  //设置用户数据
  TF->setData(0, Qt::UserRole, AddLayer::tf);

  //调用qrviz类的接口创建图层对象实体并取得其指针
  rviz::Display* display = myrviz->CreateDisplay("rviz/TF", "myTF");
  //向Hash表中插入键值对
  m_LayerHash.insert(TF, display);
  //默认调用一次修改图层属性函数
  myrviz->ModifyTFAttribute(display, checkbox->isChecked());
  return TF;
}



/**
 * @brief 向QTreeidget添加LaserScan顶层节点
 * @param QTreeWidget* treeWidget 待添加LaserScan顶层节点的QTreeWidget指针
 */
QTreeWidgetItem* work_Obstacle::add_TreeWidgetItem_LaserScan(QTreeWidget* treeWidget, QString& displayName, QString& topicName)
{
  QTreeWidgetItem* LaserScan = new QTreeWidgetItem(QStringList() << displayName);
  //设置图标
  LaserScan->setIcon(0, QIcon("://rViz/LaserScan.png"));
  //初始化一个checkbox
  QCheckBox* checkbox = new QCheckBox();
  //设置复选框默认选中状态
  checkbox->setCheckState(Qt::Checked);
  //连接checkbox状态改变信号与修改激光扫描图层属性槽函数
  connect(checkbox, &QCheckBox::stateChanged, this, &work_Obstacle::slot_modify_LaserScan_attribute);
  //加载根节点
  treeWidget->addTopLevelItem(LaserScan);
  //添加checkbox于根节点第一列
  treeWidget->setItemWidget(LaserScan, 1, checkbox);
  //设置默认展开(默认展开应写在加载根节点后)
  //LaserScan->setExpanded(true);
  //设置用户数据
  LaserScan->setData(0, Qt::UserRole, AddLayer::laserscan);

  /*
   * 定义子节点1 ———— topic
   */
  QTreeWidgetItem* LaserTopic = new QTreeWidgetItem(QStringList() << "话题");
  //添加子节点1
  LaserScan->addChild(LaserTopic);
  //初始化一个combobox
  QComboBox* combobox = new QComboBox();
  combobox->addItems(AddLayer::getTopicListByLayerCode(AddLayer::laserscan));                           //添加项
  combobox->setCurrentIndex(-1);                                                                        //设置默认不显示项
  combobox->setCurrentText(topicName);                                                                  //设置项(需要注意的是，如果topicName不匹配任何一个已经添加好的项，那么setCurrentText()方法将不起作用)
  combobox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);                              //设置尺寸调整策略为适应最小内容
  combobox->setEditable(true);                                                                          //设置可编辑
  //添加combobox于子节点1的第一列
  treeWidget->setItemWidget(LaserTopic, 1, combobox);
  //连接combobox当前文本改变信号与修改激光扫描图层属性槽函数
  connect(combobox, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_modify_LaserScan_attribute);

  /*
   * 定义子节点2 ———— style
   */
  QTreeWidgetItem* LaserRenderStyle = new QTreeWidgetItem(QStringList() << "渲染模式");
  //添加子节点2
  LaserScan->addChild(LaserRenderStyle);
  //初始化一个combobox
  QComboBox* StyleComboBox = new QComboBox();
  StyleComboBox->addItems(QStringList() << "Points" << "Squares" << "Flat Squares" << "Spheres" << "Boxes");  //添加项
  StyleComboBox->setEditable(false);                                                                          //设置不可编辑
  StyleComboBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);                               //设置尺寸调整策略为适应最小内容
  //添加combobox于子节点2的第一列
  treeWidget->setItemWidget(LaserRenderStyle, 1, StyleComboBox);
  //连接combobox当前文本改变信号与修改激光扫描图层属性槽函数
  connect(StyleComboBox, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_modify_LaserScan_attribute);
  //连接combobox当前文本改变信号与激光点云图层渲染风格改变槽函数
  connect(StyleComboBox, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_onStyleComboBox_text_changed);

  /*
   * 定义子节点3 ———— size
   */
  QTreeWidgetItem* LaserPointSize = new QTreeWidgetItem(QStringList() << "大小 (Pixels)");
  //添加子节点3
  LaserScan->addChild(LaserPointSize);
  //初始化一个lineedit
  QLineEdit* PointSizeLineEdit = new QLineEdit("3");
  //正则表达式限制只能输入正浮点数
  PointSizeLineEdit->setValidator(new QRegExpValidator(QRegExp("^(([0-9]+\\.[0-9]*[1-9][0-9]*)|([0-9]*[1-9][0-9]*\\.[0-9]+)|([0-9]*[1-9][0-9]*)|0)$")));
  //设置最小宽度
  PointSizeLineEdit->setMinimumWidth(150);
  //添加lineedit于子节点3第一列
  treeWidget->setItemWidget(LaserPointSize, 1, PointSizeLineEdit);
  //连接lineedit文本改变信号与修改激光扫描图层属性槽函数
  connect(PointSizeLineEdit, &QLineEdit::textChanged, this, &work_Obstacle::slot_modify_LaserScan_attribute);
  //连接lineedit编辑结束信号与数据处理槽函数
  connect(PointSizeLineEdit, &QLineEdit::editingFinished, this, &work_Obstacle::slot_onPointSizeLineEdit_edit_finished);

  //调用qrviz类的接口创建图层对象实体并取得其指针
  rviz::Display* display = myrviz->CreateDisplay("rviz/LaserScan", "myLaserScan");
  //向Hash表中插入键值对
  m_LayerHash.insert(LaserScan, display);
  //默认调用一次修改图层属性函数
  myrviz->ModifyLaserScanAttribute(display, combobox->currentText(), StyleComboBox->currentText(), PointSizeLineEdit->text().toDouble(), checkbox->isChecked());

  return LaserScan;
}


/**
 * @brief 向QTreeidget添加PointCloud2顶层节点
 * @param QTreeWidget* treeWidget 待添加PointCloud2顶层节点的QTreeWidget指针
 */
QTreeWidgetItem* work_Obstacle::add_TreeWidgetItem_PointCloud2(QTreeWidget* treeWidget, QString& displayName, QString& topicName)
{
  //qDebug() << displayName << topicName;
  //topicName = AddLayer::getTopicName();//重新获取当下话题
  QTreeWidgetItem* PointCloud2 = new QTreeWidgetItem(QStringList() << displayName);
  //设置图标
  PointCloud2->setIcon(0, QIcon("://rViz/PointCloud2.png"));
  //初始化一个checkbox
  QCheckBox* checkBox = new QCheckBox();
  //设置复选框默认选中状态
  checkBox->setCheckState(Qt::Checked);
  //连接checkbox状态改变信号与修改点云2图层属性槽函数
  connect(checkBox, &QCheckBox::stateChanged, this, &work_Obstacle::slot_modify_PointCloud2_attribute);
  //加载根节点
  treeWidget->addTopLevelItem(PointCloud2);
  //添加checkbox于根节点第一列
  treeWidget->setItemWidget(PointCloud2, 1, checkBox);
  //设置默认展开(默认展开应写在加载根节点后)
  //PointCloud2->setExpanded(true);
  //设置用户数据
  PointCloud2->setData(0, Qt::UserRole, AddLayer::pointcloud2);

  /*
   * 定义子节点1 ———— topic
   */
  QTreeWidgetItem* PointCloud2Topic = new QTreeWidgetItem(QStringList() << "话题");
  //添加子节点1
  PointCloud2->addChild(PointCloud2Topic);
  //初始化一个combobox
  QComboBox* combobox = new QComboBox();
  combobox->addItems(AddLayer::getTopicListByLayerCode(AddLayer::pointcloud2)); //添加项
  //qDebug() << "Topic List: " << AddLayer::getTopicListByLayerCode(AddLayer::pointcloud2);//打印出话题

  combobox->setCurrentIndex(-1);                                              //设置默认不显示项
  combobox->setCurrentText(topicName);                                        //设置项(需要注意的是，如果topicName不匹配任何一个已经添加好的项，那么setCurrentText()方法将不起作用)
  combobox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);    //设置尺寸调整策略为适应最小内容
  combobox->setEditable(true);                                                //设置可编辑
  //添加combobox于子节点1的第一列
  treeWidget->setItemWidget(PointCloud2Topic, 1, combobox);
  //连接combobox当前文本改变信号与修改点云2图层属性槽函数
  connect(combobox, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_modify_PointCloud2_attribute);

  /*
   * 定义子节点2 ———— style
   */
  QTreeWidgetItem* PointCloud2RenderStyle = new QTreeWidgetItem(QStringList() << "渲染模式");
  //添加子节点2
  PointCloud2->addChild(PointCloud2RenderStyle);
  //初始化一个combobox
  QComboBox* StyleComboBox = new QComboBox();
  StyleComboBox->addItems(QStringList() << "Points" << "Squares" << "Flat Squares" << "Spheres" << "Boxes");   //添加项
  StyleComboBox->setEditable(false);                                                                           //设置不可编辑
  StyleComboBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);                                //设置尺寸调整策略为适应最小内容
  //添加combobox于子节点2的第一列
  treeWidget->setItemWidget(PointCloud2RenderStyle, 1, StyleComboBox);
  //连接combobox当前文本改变信号与修改点云2图层属性槽函数
  connect(StyleComboBox, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_modify_PointCloud2_attribute);
  //连接combobox当前文本改变信号与点云2图层渲染风格改变槽函数
  connect(StyleComboBox, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_onStyleComboBox_text_changed);

  /*
   * 定义子节点3 ———— size
   */
  QTreeWidgetItem* PointCloud2PointSize = new QTreeWidgetItem(QStringList() << "大小 (Pixels)");
  //添加子节点3
  PointCloud2->addChild(PointCloud2PointSize);
  //初始化一个lineedit
  QLineEdit* PointSizeLineEdit = new QLineEdit("3");
  //正则表达式限制只能输入正浮点数
  PointSizeLineEdit->setValidator(new QRegExpValidator(QRegExp("^(([0-9]+\\.[0-9]*[1-9][0-9]*)|([0-9]*[1-9][0-9]*\\.[0-9]+)|([0-9]*[1-9][0-9]*)|0)$")));
  //设置最小宽度
  PointSizeLineEdit->setMinimumWidth(150);
  //添加lineedit于子节点3第一列
  treeWidget->setItemWidget(PointCloud2PointSize, 1, PointSizeLineEdit);
  //连接lineedit文本改变信号与修改点云2图层属性槽函数
  connect(PointSizeLineEdit, &QLineEdit::textChanged, this, &work_Obstacle::slot_modify_PointCloud2_attribute);
  //连接lineedit编辑结束信号与数据处理槽函数
  connect(PointSizeLineEdit, &QLineEdit::editingFinished, this, &work_Obstacle::slot_onPointSizeLineEdit_edit_finished);

  //调用qrviz类的接口创建图层对象实体并取得其指针
  rviz::Display* display = myrviz->CreateDisplay("rviz/PointCloud2", "myPointCloud2");
  //向Hash表中插入键值对
  m_LayerHash.insert(PointCloud2, display);
  //默认调用一次修改图层属性函数
  myrviz->ModifyPointCloud2Attribute(display, combobox->currentText(), StyleComboBox->currentText(), PointSizeLineEdit->text().toDouble(), checkBox->isChecked());

  return PointCloud2;
}


/**
 * @brief 向QTreeidget添加RobotModel顶层节点
 * @param QTreeWidget* treeWidget 待添加RobotModel顶层节点的QTreeWidget指针
 */
QTreeWidgetItem* work_Obstacle::add_TreeWidgetItem_RobotModel(QTreeWidget *treeWidget, QString& displayName)
{
  QTreeWidgetItem* RobotModel = new QTreeWidgetItem(QStringList() << displayName);
  //设置图标
  RobotModel->setIcon(0, QIcon("://rViz/RobotModel.png"));
  //初始化一个checkbox
  QCheckBox* checkbox = new QCheckBox();
  //设置复选框默认选中
  checkbox->setCheckState(Qt::Checked);
  //连接checkbox状态改变信号与修改机器人模型图层属性槽函数
  connect(checkbox, &QCheckBox::stateChanged, this, &work_Obstacle::slot_modify_RobotModel_attribute);
  //加载根节点
  treeWidget->addTopLevelItem(RobotModel);
  //添加checkbox于根节点第一列
  treeWidget->setItemWidget(RobotModel, 1, checkbox);
  //设置用户数据
  RobotModel->setData(0, Qt::UserRole, AddLayer::robotmodel);

  //调用qrviz类的接口创建图层对象实体并取得其指针
  rviz::Display* display = myrviz->CreateDisplay("rviz/RobotModel", "myRobotModel");
  //向Hash表中插入键值对
  m_LayerHash.insert(RobotModel, display);
  //默认调用一次修改图层属性函数
  myrviz->ModifyRobotModelAttribute(display, checkbox->isChecked());
  return RobotModel;
}


/**
 * @brief 向QTreeidget添加Map顶层节点
 * @param QTreeWidget* treeWidget 待添加Map顶层节点的QTreeWidget指针
 */
QTreeWidgetItem* work_Obstacle::add_TreeWidgetItem_Map(QTreeWidget* treeWidget, QString& displayName, QString& topicName)
{
  QTreeWidgetItem* Map = new QTreeWidgetItem(QStringList() << displayName);
  //设置图标
  Map->setIcon(0, QIcon("://rViz/Map.png"));
//  Map->setFlags(Map->flags() | Qt::ItemIsUserCheckable);    //为顶层节点设置Qt::ItemIsUserCheckable标志同样可以添加复选框
//  Map->setCheckState(1, Qt::Checked);                       //但QTreeWidget没有提供一个专门的信号来检测QTreeWidgetItem的选中状态改变，不过我们可以使用itemChanged(QTreeWidgetItem * item, int column)这个信号
  //初始化一个checkbox
  QCheckBox* checkbox = new QCheckBox();
  //设置复选框默认选中
  checkbox->setChecked(Qt::Checked);
  //连接checkbox状态改变信号与修改地图图层属性槽函数
  connect(checkbox, &QCheckBox::stateChanged, this, &work_Obstacle::slot_modify_Map_attribute);
  //加载根节点
  treeWidget->addTopLevelItem(Map);
  //设置默认展开(默认展开应写在加载根节点后)
  //Map->setExpanded(true);
  //添加checkbox于根节点第一列
  treeWidget->setItemWidget(Map, 1, checkbox);
  //设置用户数据
  Map->setData(0, Qt::UserRole, AddLayer::map);

  /*
   * 定义子节点1 ———— topic
   */
  QTreeWidgetItem* MapTopic = new QTreeWidgetItem(QStringList() << "话题");
  //添加子节点1
  Map->addChild(MapTopic);
  //初始化一个combobox
  QComboBox* combobox1 = new QComboBox();
  combobox1->addItems(AddLayer::getTopicListByLayerCode(AddLayer::map));         //添加项
  combobox1->setCurrentIndex(-1);                                               //设置默认不显示项
  combobox1->setCurrentText(topicName);                                         //设置项(需要注意的是，如果topicName不匹配任何一个已经添加好的项，那么setCurrentText()方法将不起作用)
  combobox1->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);     //设置尺寸调整策略为适应最小内容
  combobox1->setEditable(true);                                                 //设置可编辑
  //添加combobox于子节点1的第一列
  treeWidget->setItemWidget(MapTopic, 1, combobox1);
  //连接combobox当前文本改变信号与修改地图图层属性槽函数
  connect(combobox1, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_modify_Map_attribute);

  /*
   * 定义子节点2 ———— color scheme
   */
  QTreeWidgetItem* MapColorScheme = new QTreeWidgetItem(QStringList() << "配色方案");
  //添加子节点2
  Map->addChild(MapColorScheme);
  //初始化一个combobox
  QComboBox* combobox2 = new QComboBox();
  combobox2->addItems(QStringList() << "map" << "costmap" << "raw");          //添加项
  combobox2->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);   //设置尺寸调整策略为适应最小内容
  combobox2->setEditable(false);                                              //设置不可编辑
  //添加combobox于子节点2的第一列
  treeWidget->setItemWidget(MapColorScheme, 1, combobox2);
  //连接combobox当前文本改变信号与修改地图图层属性槽函数
  connect(combobox2, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_modify_Map_attribute);

  //调用qrviz类的接口创建图层对象实体并取得其指针
  rviz::Display* display = myrviz->CreateDisplay("rviz/Map", "myMap");
  //向Hash表中插入键值对
  m_LayerHash.insert(Map, display);
  //默认调用一次修改图层属性函数
  myrviz->ModifyMapAttribute(display, combobox1->currentText(), combobox2->currentText(), checkbox->isChecked());
  //返回对象指针
  return Map;
}


/**
 * @brief 向QTreeidget添加Path顶层节点
 * @param QTreeWidget* treeWidget 待添加Path顶层节点的QTreeWidget指针
 */
QTreeWidgetItem* work_Obstacle::add_TreeWidgetItem_Path(QTreeWidget* treeWidget, QString& displayName, QString& topicName, QColor color)
{
  QTreeWidgetItem* Path = new QTreeWidgetItem(QStringList() << displayName);
  //设置图标
  Path->setIcon(0, QIcon("://rViz/Path.png"));
  //初始化一个checkbox
  QCheckBox* checkbox = new QCheckBox();
  //设置复选框默认选中
  checkbox->setCheckState(Qt::Checked);
  //连接checkbox状态改变信号与修改路径图层属性槽函数
  connect(checkbox, &QCheckBox::stateChanged, this, &work_Obstacle::slot_modify_Path_attribute);
  //加载根节点
  treeWidget->addTopLevelItem(Path);
  //设置默认展开(默认展开应写在加载根节点后)
  //Path->setExpanded(true);
  //添加checkbox于根节点第一列
  treeWidget->setItemWidget(Path, 1, checkbox);
  //设置用户数据
  Path->setData(0, Qt::UserRole, AddLayer::path);

  /*
   * 定义子节点1 ———— topic
   */
  QTreeWidgetItem* PathTopic = new QTreeWidgetItem(QStringList() << "话题");
  //添加子节点1
  Path->addChild(PathTopic);
  //初始化一个combobox
  QComboBox* combobox = new QComboBox();
  combobox->addItems(AddLayer::getTopicListByLayerCode(AddLayer::path));                                        //添加项
  combobox->setCurrentIndex(-1);                                                                                //设置默认不显示项
  combobox->setCurrentText(topicName);                                                                          //设置项(需要注意的是，如果topicName不匹配任何一个已经添加好的项，那么setCurrentText()方法将不起作用)
  combobox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);                                      //设置尺寸调整策略为适应最小内容
  combobox->setEditable(true);                                                                                  //设置可编辑
  //添加combobox于子节点1的第一列
  treeWidget->setItemWidget(PathTopic, 1, combobox);
  //连接combobox当前文本改变信号与修改路径图层属性槽函数
  connect(combobox, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_modify_Path_attribute);

  /*
   * 定义子节点2 ———— color
   */
  QTreeWidgetItem* PathColor = new QTreeWidgetItem(QStringList() << "路径颜色");
  //添加子节点2
  Path->addChild(PathColor);
  //初始化一个MyPalette对象
  MyPalette* palette = new MyPalette();
  //设置初始颜色
  palette->setColor(color);
  //添加mypalette于子节点2的第一列
  treeWidget->setItemWidget(PathColor, 1, palette);
  //连接palette颜色改变信号与修改路径图层属性槽函数
  connect(palette, &MyPalette::colorchanged, this, &work_Obstacle::slot_modify_Path_attribute);

  //调用qrviz类的接口创建图层对象实体并取得其指针
  rviz::Display* display = myrviz->CreateDisplay("rviz/Path", "myPath");
  //向Hash表中插入键值对
  m_LayerHash.insert(Path, display);
  //默认调用一次修改图层属性函数
  myrviz->ModifyPathAttribute(display, combobox->currentText(), palette->currentColor(), checkbox->isChecked());

  return Path;
}


/**
 * @brief 向QTreeidget添加PointStamped顶层节点
 * @param QTreeWidget* treeWidget 待添加PointStamped顶层节点的QTreeWidget指针
 */
QTreeWidgetItem* work_Obstacle::add_TreeWidgetItem_PointStamped(QTreeWidget* treeWidget, QString& displayName, QString& topicName)
{
  //PointStamped代表了一个带有参考坐标系和时间戳的点
  QTreeWidgetItem* PointStamped = new QTreeWidgetItem(QStringList() << displayName);
  //设置图标
  PointStamped->setIcon(0, QIcon("://rViz/PointStamped.png"));
  //初始化一个checkbox
  QCheckBox* checkbox = new QCheckBox();
  //设置复选框默认选中
  checkbox->setCheckState(Qt::Checked);
  //连接checkbox状态改变信号与修改带点戳图层属性槽函数
  connect(checkbox, &QCheckBox::stateChanged, this, &work_Obstacle::slot_modify_PointStamped_attribute);
  //加载根节点
  treeWidget->addTopLevelItem(PointStamped);
  //设置默认展开(默认展开应写在加载根节点后)
  //PointStamped->setExpanded(true);
  //添加checkbox于根节点第一列
  treeWidget->setItemWidget(PointStamped, 1, checkbox);
  //设置用户数据
  PointStamped->setData(0, Qt::UserRole, AddLayer::pointstamped);


  /*
   * 定义子节点1 ———— topic
   */
  QTreeWidgetItem* PointStampedTopic = new QTreeWidgetItem(QStringList() << "话题");
  //添加子节点1
  PointStamped->addChild(PointStampedTopic);
  //初始化一个combobox
  QComboBox* combobox = new QComboBox();
  combobox->addItems(AddLayer::getTopicListByLayerCode(AddLayer::pointstamped));    //添加项
  combobox->setCurrentIndex(-1);                                              //设置默认不显示项
  combobox->setCurrentText(topicName);                                        //设置项(需要注意的是，如果topicName不匹配任何一个已经添加好的项，那么setCurrentText()方法将不起作用)
  combobox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);    //设置尺寸调整策略为适应最小内容
  combobox->setEditable(true);                                                //设置可编辑
  //添加combobox于子节点1的第一列
  treeWidget->setItemWidget(PointStampedTopic, 1, combobox);
  //连接combobox当前文本改变信号与修改带点戳图层属性槽函数
  connect(combobox, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_modify_PointStamped_attribute);

  /*
   * 定义子节点2 ———— color
   */
  QTreeWidgetItem* PointStampedColor = new QTreeWidgetItem(QStringList() << "点颜色");
  //添加子节点2
  PointStamped->addChild(PointStampedColor);
  //初始化一个MyPalette对象
  MyPalette* palette = new MyPalette();
  //设置初始颜色
  palette->setColor(204, 41, 204);
  //添加mypalette于子节点2的第一列
  treeWidget->setItemWidget(PointStampedColor, 1, palette);
  //连接mypalette颜色改变信号与修改带点戳图层属性槽函数
  connect(palette, &MyPalette::colorchanged, this, &work_Obstacle::slot_modify_PointStamped_attribute);

  //调用qrviz类的接口创建图层对象实体并取得其指针
  rviz::Display* display = myrviz->CreateDisplay("rviz/PointStamped", "myPointStamped");
  //向Hash表中插入键值对
  m_LayerHash.insert(PointStamped, display);
  //默认调用一次修改图层属性函数
  myrviz->ModifyPointStampedAttribute(display, combobox->currentText(), palette->currentColor(), checkbox->isChecked());

  return PointStamped;
}


/**
 * @brief 向QTreeidget添加Marker顶层节点
 * @param QTreeWidget* treeWidget 待添加Marker顶层节点的QTreeWidget指针
 */
QTreeWidgetItem* work_Obstacle::add_TreeWidgetItem_Marker(QTreeWidget* treeWidget, QString& displayName, QString& topicName)
{
  QTreeWidgetItem* Marker = new QTreeWidgetItem(QStringList() << displayName);
  //设置图标
  Marker->setIcon(0, QIcon("://rViz/Marker.png"));
  //初始化一个checkbox
  QCheckBox* checkbox = new QCheckBox();
  //设置复选框默认选中
  checkbox->setCheckState(Qt::Checked);
  //加载根节点
  treeWidget->addTopLevelItem(Marker);
  //设置默认展开(默认展开应写在加载根节点后)
  //Marker->addChild(MarkerCBox);
  //添加checkbox于根节点的第一列
  treeWidget->setItemWidget(Marker, 1, checkbox);
  //连接checkbox状态改变信号与修改标记图层属性槽函数
  connect(checkbox, &QCheckBox::stateChanged, this, &work_Obstacle::slot_modify_Marker_attribute);
  //设置用户数据
  Marker->setData(0, Qt::UserRole, AddLayer::marker);

  /*
   * 定义子节点1 ———— topic
   */
  QTreeWidgetItem* MarkerTopic = new QTreeWidgetItem(QStringList() << "标记话题");
  //添加子节点1
  Marker->addChild(MarkerTopic);
  //初始化一个combobox
  QComboBox* combobox = new QComboBox();
  combobox->addItems(AddLayer::getTopicListByLayerCode(AddLayer::marker));            //添加项
  combobox->setCurrentIndex(-1);                                                      //设置默认不显示项
  combobox->setCurrentText(topicName);                                                //设置项(需要注意的是，如果topicName不匹配任何一个已经添加好的项，那么setCurrentText()方法将不起作用)
  combobox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);            //设置尺寸调整策略为适应最小内容
  combobox->setEditable(true);                                                        //设置可编辑
  //添加combobox于子节点1的第一列
  treeWidget->setItemWidget(MarkerTopic, 1, combobox);
  //连接combobox当前文本改变信号与修改标记图层属性槽函数
  connect(combobox, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_modify_Marker_attribute);

  //调用qrviz类的接口创建图层对象实体并取得其指针
  rviz::Display* display = myrviz->CreateDisplay("rviz/Marker", "myMarker");
  //向Hash表中插入键值对
  m_LayerHash.insert(Marker, display);
  //默认调用一次修改图层属性函数
  myrviz->ModifyMarkerAttribute(display, combobox->currentText(), checkbox->isChecked());
  return Marker;
}


/**
 * @brief 向QTreeidget添加MarkerArray顶层节点
 * @param QTreeWidget* treeWidget 待添加MarkerArray顶层节点的QTreeWidget指针
 */
QTreeWidgetItem* work_Obstacle::add_TreeWidgetItem_MarkerArray(QTreeWidget* treeWidget, QString& displayName, QString& topicName)
{
  QTreeWidgetItem* MarkerArray = new QTreeWidgetItem(QStringList() << displayName);
  //设置图标
  MarkerArray->setIcon(0, QIcon("://rViz/MarkerArray.png"));
  //初始化一个checkbox
  QCheckBox* checkbox = new QCheckBox();
  //设置复选框默认选中
  checkbox->setCheckState(Qt::Checked);
  //加载根节点
  treeWidget->addTopLevelItem(MarkerArray);
  //设置默认展开
  //MarkerArray->setExpanded(true);
  //添加checkbox于根节点的第一列
  treeWidget->setItemWidget(MarkerArray, 1, checkbox);
  //连接checkbox状态改变信号与修改标记数组图层属性槽函数
  connect(checkbox, &QCheckBox::stateChanged, this, &work_Obstacle::slot_modify_MarkerArray_attribute);
  //设置用户数据
  MarkerArray->setData(0, Qt::UserRole, AddLayer::markerarray);

  /*
   * 定义子节点1 ———— topic
   */
  QTreeWidgetItem* MarkerArrayTopic = new QTreeWidgetItem(QStringList() << "标记话题");
  //添加子节点1
  MarkerArray->addChild(MarkerArrayTopic);
  //初始化一个combobox
  QComboBox* combobox = new QComboBox();
  combobox->addItems(AddLayer::getTopicListByLayerCode(AddLayer::markerarray));   //添加项
  combobox->setCurrentIndex(-1);                                              //设置默认不显示项
  combobox->setCurrentText(topicName);                                        //设置项(需要注意的是，如果topicName不匹配任何一个已经添加好的项，那么setCurrentText()方法将不起作用)
  combobox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);    //设置尺寸调整策略为适应最小内容
  combobox->setEditable(true);                                                //设置可编辑
  //添加combobox于子节点1的第一列
  treeWidget->setItemWidget(MarkerArrayTopic, 1, combobox);
  //连接combobox当前文本改变信号与修改标记数组图层属性槽函数
  connect(combobox, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_modify_MarkerArray_attribute);

  //调用qrviz类的接口创建图层对象实体并取得其指针
  rviz::Display* display = myrviz->CreateDisplay("rviz/MarkerArray", "myMarkerArray");
  //向Hash表中插入键值对
  m_LayerHash.insert(MarkerArray, display);
  //默认调用一次修改图层属性函数
  myrviz->ModifyMarkerArrayAttribute(display, combobox->currentText(), checkbox->isChecked());

  return MarkerArray;
}
/**
 * @brief 向QTreeidget添加Image图像顶层节点
 * @param QTreeWidget* treeWidget 待添加Image顶层节点的QTreeWidget指针
 */
QTreeWidgetItem *work_Obstacle::add_TreeWidgetItem_Image(QTreeWidget *treeWidget, QString &displayName, QString &topicName)
{
    QTreeWidgetItem* Image = new QTreeWidgetItem(QStringList() << displayName);
    //设置图标
    Image->setIcon(0, QIcon("://rViz/Image.png"));
    //初始化一个checkbox
    QCheckBox* checkbox = new QCheckBox();
    //设置复选框默认选中
    checkbox->setCheckState(Qt::Unchecked);
    //加载根节点
    treeWidget->addTopLevelItem(Image);
    //设置默认展开
    //MarkerArray->setExpanded(true);
    //添加checkbox于根节点的第一列
    treeWidget->setItemWidget(Image, 1, checkbox);
    //连接checkbox状态改变信号与修改标记数组图层属性槽函数
    connect(checkbox, &QCheckBox::stateChanged, this, &work_Obstacle::slot_modify_Image_attribute);
    //设置用户数据？
    Image->setData(0, Qt::UserRole, AddLayer::image);

    /*
     * 定义子节点1 ———— topic
     */
    QTreeWidgetItem* ImageTopic = new QTreeWidgetItem(QStringList() << "图像Image话题");
    //添加子节点1
    Image->addChild(ImageTopic);
    //初始化一个combobox
    QComboBox* combobox = new QComboBox();
    combobox->addItems(AddLayer::getTopicListByLayerCode(AddLayer::image));     //添加项
    combobox->setCurrentIndex(-1);                                              //设置默认不显示项
    combobox->setCurrentText(topicName);                                        //设置项(需要注意的是，如果topicName不匹配任何一个已经添加好的项，那么setCurrentText()方法将不起作用)
    combobox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);    //设置尺寸调整策略为适应最小内容
    combobox->setEditable(true);                                                //设置可编辑
    //添加combobox于子节点1的第一列
    treeWidget->setItemWidget(ImageTopic, 1, combobox);
    //连接combobox当前文本改变信号与修改标记数组图层属性槽函数
    connect(combobox, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_modify_Image_attribute);

    //调用qrviz类的接口创建图层对象实体并取得其指针
    rviz::Display* display = myrviz->CreateDisplay("rviz/Image", "myImage");
    //向Hash表中插入键值对
    m_LayerHash.insert(Image, display);
    //默认调用一次修改图层属性函数
    myrviz->ModifyImageAttribute(display, combobox->currentText(), checkbox->isChecked());

    return Image;
}

QTreeWidgetItem *work_Obstacle::add_TreeWidgetItem_Pose(QTreeWidget *treeWidget, QString &displayName, QString &topicName)
{
    QTreeWidgetItem* Pose = new QTreeWidgetItem(QStringList() << displayName);
    //设置图标
    Pose->setIcon(0, QIcon("://rViz/Pose.png"));
    //初始化一个checkbox
    QCheckBox* checkbox = new QCheckBox();
    //设置复选框默认选中
    checkbox->setCheckState(Qt::Checked);
    //加载根节点
    treeWidget->addTopLevelItem(Pose);
    //设置默认展开
    //MarkerArray->setExpanded(true);
    //添加checkbox于根节点的第一列
    treeWidget->setItemWidget(Pose, 1, checkbox);
    //连接checkbox状态改变信号与修改标记数组图层属性槽函数
    connect(checkbox, &QCheckBox::stateChanged, this, &work_Obstacle::slot_modify_Pose_attribute);
    //设置用户数据？
    Pose->setData(0, Qt::UserRole, AddLayer::pose);

    /*
     * 定义子节点1 ———— topic
     */
    QTreeWidgetItem* PoseTopic = new QTreeWidgetItem(QStringList() << "Pose话题");
    //添加子节点1
    Pose->addChild(PoseTopic);
    //初始化一个combobox
    QComboBox* combobox = new QComboBox();
    combobox->addItems(AddLayer::getTopicListByLayerCode(AddLayer::pose));     //添加项
    combobox->setCurrentIndex(-1);                                              //设置默认不显示项
    combobox->setCurrentText(topicName);                                        //设置项(需要注意的是，如果topicName不匹配任何一个已经添加好的项，那么setCurrentText()方法将不起作用)
    combobox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);    //设置尺寸调整策略为适应最小内容
    combobox->setEditable(true);                                                //设置可编辑
    //添加combobox于子节点1的第一列
    treeWidget->setItemWidget(PoseTopic, 1, combobox);
    //连接combobox当前文本改变信号与修改标记数组图层属性槽函数
    connect(combobox, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_modify_Pose_attribute);

    //调用qrviz类的接口创建图层对象实体并取得其指针
    rviz::Display* display = myrviz->CreateDisplay("rviz/Pose", "myPose");
    //向Hash表中插入键值对
    m_LayerHash.insert(Pose, display);
    //默认调用一次修改图层属性函数(这里需要改一下)
    myrviz->ModifyPoseAttribute(display, combobox->currentText(),QColor(20,20,20), checkbox->isChecked());

    return Pose;
}

QTreeWidgetItem *work_Obstacle::add_TreeWidgetItem_BoundingBoxArray(QTreeWidget *treeWidget, QString &displayName, QString &topicName)
{
    QTreeWidgetItem* BoundingBoxArray = new QTreeWidgetItem(QStringList() << displayName);
    //设置图标
    BoundingBoxArray->setIcon(0, QIcon("://rViz/BoundingBoxArray.png"));
    //初始化一个checkbox
    QCheckBox* checkbox = new QCheckBox();
    //设置复选框默认选中
    checkbox->setCheckState(Qt::Unchecked);
    //加载根节点
    treeWidget->addTopLevelItem(BoundingBoxArray);
    //设置默认展开
    //MarkerArray->setExpanded(true);
    //添加checkbox于根节点的第一列
    treeWidget->setItemWidget(BoundingBoxArray, 1, checkbox);
    //连接checkbox状态改变信号与修改标记数组图层属性槽函数
    connect(checkbox, &QCheckBox::stateChanged, this, &work_Obstacle::slot_modify_BoundingBoxArray_attribute);
    //设置用户数据？
    BoundingBoxArray->setData(0, Qt::UserRole, AddLayer::boundingboxarray);

    /*
     * 定义子节点1 ———— topic
     */
    QTreeWidgetItem* BoundingBoxArrayTopic = new QTreeWidgetItem(QStringList() << "BoundingBoxArray话题");
    //添加子节点1
    BoundingBoxArray->addChild(BoundingBoxArrayTopic);
    //初始化一个combobox
    QComboBox* combobox = new QComboBox();
    combobox->addItems(AddLayer::getTopicListByLayerCode(AddLayer::boundingboxarray));     //添加项
    combobox->setCurrentIndex(-1);                                              //设置默认不显示项
    combobox->setCurrentText(topicName);                                        //设置项(需要注意的是，如果topicName不匹配任何一个已经添加好的项，那么setCurrentText()方法将不起作用)
    combobox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);    //设置尺寸调整策略为适应最小内容
    combobox->setEditable(true);                                                //设置可编辑
    //添加combobox于子节点1的第一列
    treeWidget->setItemWidget(BoundingBoxArrayTopic, 1, combobox);
    //连接combobox当前文本改变信号与修改标记数组图层属性槽函数
    connect(combobox, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_modify_BoundingBoxArray_attribute);

    //调用qrviz类的接口创建图层对象实体并取得其指针
    rviz::Display* display = myrviz->CreateDisplay("jsk_rviz_plugin/BoundingBoxArray", "myBoundingBoxArray");
    //向Hash表中插入键值对
    m_LayerHash.insert(BoundingBoxArray, display);
    //默认调用一次修改图层属性函数(这里需要改一下)
    myrviz->ModifyBoundingBoxArrayAttribute(display, combobox->currentText(), checkbox->isChecked());

    return BoundingBoxArray;
}
//QTreeWidgetItem* work_Obstacle::add_TreeWidgetItem_Image(QTreeWidget *treeWidget, QString &displayName, QString &topicName)
//{
//    QTreeWidgetItem* Image = new QTreeWidgetItem(QStringList() << displayName);
//    // 设置图标
//    Image->setIcon(0, QIcon("://rViz/Image.png"));
//    // 初始化一个checkbox
//    QCheckBox* checkbox = new QCheckBox();
//    // 设置复选框默认选中
//    checkbox->setCheckState(Qt::Unchecked);
//    // 加载根节点
//    treeWidget->addTopLevelItem(Image);
//    // 设置默认展开
//    Image->setExpanded(true); // 如果需要自动展开
//    // 添加checkbox到根节点的第一列
//    treeWidget->setItemWidget(Image, 1, checkbox);
//    // 连接checkbox状态改变信号与修改图层属性槽函数
//    connect(checkbox, &QCheckBox::stateChanged, this, &work_Obstacle::slot_modify_Image_attribute);
//    // 设置用户数据？
//    Image->setData(0, Qt::UserRole, AddLayer::image);
//    // 定义子节点1 ———— topic
//    QTreeWidgetItem* ImageTopic = new QTreeWidgetItem(QStringList() << "图像Image话题");
//    // 添加子节点1
//    Image->addChild(ImageTopic);
//    // 初始化一个combobox
//    QComboBox* combobox = new QComboBox();
//    combobox->addItems(AddLayer::getTopicListByLayerCode(AddLayer::image)); // 添加项
//    combobox->setCurrentIndex(-1); // 设置默认不显示项
//    combobox->setCurrentText(topicName); // 设置项
//    combobox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength); // 设置尺寸调整策略为适应最小内容
//    combobox->setEditable(true); // 设置可编辑
//    // 添加combobox到子节点1的第一列
//    treeWidget->setItemWidget(ImageTopic, 1, combobox);

//    // 连接combobox当前文本改变信号与修改图层属性槽函数
//    connect(combobox, &QComboBox::currentTextChanged, this, &work_Obstacle::slot_modify_Image_attribute);

//    // 调用qrviz类的接口创建图层对象实体并取得其指针
//    rviz::Display* display = myrviz->CreateDisplay("rviz/Image", "myImage");

//    // 向Hash表中插入键值对
//    m_LayerHash.insert(Image, display);

//    // 默认调用一次修改图层属性函数
//    myrviz->ModifyImageAttribute(display, combobox->currentText(), checkbox->isChecked());

//    // 确保在删除该项时能够安全删除图层
//    // 使用QPointer来确保指针有效性
//    QPointer<rviz::Display> safeDisplay = display;

//    // 在Image项被删除时，自动清理关联的图层
//    // 手动清理图层资源
//    connect(treeWidget, &QTreeWidget::itemChanged, this, [=](QTreeWidgetItem* changedItem, int column) {
//        if (changedItem == Image && column == 0) {  // 如果图层被删除
//            if (display) {
//                // 清理图层
//                delete display; // 销毁图层对象
//            }
//        }
//    });
//    return Image;
//}

/***********************************************************************************************
 *                                    修改图层相关属性方法定义                                     *
 ***********************************************************************************************/
/**
 * @brief 修改网格图层属性槽函数
 */
void work_Obstacle::slot_modify_grid_attribute()
{
  QTreeWidgetItem* grid;
  QCheckBox* checkBox;
  QSpinBox* spinBox;
  MyPalette* palette;
  if(ui->treeWidget_layer->currentItem()->parent() == nullptr)
  {
    //获取顶层节点
    grid = ui->treeWidget_layer->currentItem();
  }
  else
  {
    //获取顶层节点
    grid = ui->treeWidget_layer->currentItem()->parent();
  }
  //获取顶层节点第二列单元格中的QCheckBox控件
  checkBox = qobject_cast<QCheckBox*>(ui->treeWidget_layer->itemWidget(grid, 1));
  //获取顶层节点的子节点1第二列单元格中的QSpinBox控件
  spinBox = qobject_cast<QSpinBox*>(ui->treeWidget_layer->itemWidget(grid->child(0), 1));
  //获取顶层节点的子节点2第二列单元格中的MyPalette控件
  palette = qobject_cast<MyPalette*>(ui->treeWidget_layer->itemWidget(grid->child(1), 1));
  //根据复选框是否选中决定是否启用Grid树
  if(checkBox->isChecked())
  {
    for(int index = 0; index < grid->childCount(); index++)
    {
      grid->child(index)->setDisabled(false);
    }
    spinBox->setEnabled(true);
    palette->setEnabled(true);
  }
  else
  {
    for(int index = 0; index < grid->childCount(); index++)
    {
      grid->child(index)->setDisabled(true);
    }
    spinBox->setEnabled(false);
    palette->setEnabled(false);
  }
  //修改属性
  myrviz->ModifyGridAttribute(m_LayerHash.value(grid), spinBox->value(), palette->currentColor(), checkBox->isChecked());
}



/**
 * @brief 修改TF图层属性槽函数
 */
void work_Obstacle::slot_modify_TF_attribute()
{
  QTreeWidgetItem* tf;
  QCheckBox* checkBox;
  if(ui->treeWidget_layer->currentItem()->parent() == nullptr)
  {
    //获取顶层节点
    tf = ui->treeWidget_layer->currentItem();
  }
  else
  {
    //获取顶层节点
    tf = ui->treeWidget_layer->currentItem()->parent();
  }
  //获取顶层节点第二列单元格中的QCheckBox控件
  checkBox = qobject_cast<QCheckBox*>(ui->treeWidget_layer->itemWidget(tf, 1));
  //修改属性
  myrviz->ModifyTFAttribute(m_LayerHash.value(tf), checkBox->isChecked());
}



/**
 * @brief 修改激光扫描图层属性槽函数
 */
void work_Obstacle::slot_modify_LaserScan_attribute()
{
  QTreeWidgetItem* laserscan;
  QCheckBox* checkBox;
  QComboBox* comboBox;
  QComboBox* StyleComboBox;
  QLineEdit* PointSizeLineEdit;
  if(ui->treeWidget_layer->currentItem()->parent() == nullptr)
  {
    //获取顶层节点
    laserscan = ui->treeWidget_layer->currentItem();
  }
  else
  {
    //获取顶层节点
    laserscan = ui->treeWidget_layer->currentItem()->parent();
  }
  //获取顶层节点第二列单元格中的QCheckBox控件
  checkBox = qobject_cast<QCheckBox*>(ui->treeWidget_layer->itemWidget(laserscan, 1));
  //获取顶层节点的子节点1第二列单元格中的QComboBox控件
  comboBox = qobject_cast<QComboBox*>(ui->treeWidget_layer->itemWidget(laserscan->child(0), 1));
  //获取顶层节点的子节点2第二列单元格中的QComboBox控件
  StyleComboBox = qobject_cast<QComboBox*>(ui->treeWidget_layer->itemWidget(laserscan->child(1), 1));
  //获取顶层节点的子节点3第二列单元格中的QLineEdit控件
  PointSizeLineEdit = qobject_cast<QLineEdit*>(ui->treeWidget_layer->itemWidget(laserscan->child(2), 1));
  if(checkBox->isChecked())
  {
    for(int index = 0; index < laserscan->childCount(); index++)
    {
      laserscan->child(index)->setDisabled(false);
    }
    comboBox->setEnabled(true);
    StyleComboBox->setEnabled(true);
    PointSizeLineEdit->setEnabled(true);
  }
  else
  {
    for(int index = 0; index < laserscan->childCount(); index++)
    {
      laserscan->child(index)->setDisabled(true);
    }
    comboBox->setEnabled(false);
    StyleComboBox->setEnabled(false);
    PointSizeLineEdit->setEnabled(false);
  }
  double doubleSize = PointSizeLineEdit->text().toDouble();
  if(doubleSize < 0.0001)
  {
    doubleSize = 0.0001;    //限制最小输入
  }
  myrviz->ModifyLaserScanAttribute(m_LayerHash.value(laserscan), comboBox->currentText(), StyleComboBox->currentText(), doubleSize, checkBox->isChecked());
}



/**
 * @brief 修改点云2图层属性槽函数
 */
void work_Obstacle::slot_modify_PointCloud2_attribute()
{
  QTreeWidgetItem* pointcloud2;
  QCheckBox* checkBox;
  QComboBox* comboBox;
  QComboBox* StyleComboBox;
  QLineEdit* PointSizeLineEdit;
  if(ui->treeWidget_layer->currentItem()->parent() == nullptr)
  {
    //获取顶层节点
    pointcloud2 = ui->treeWidget_layer->currentItem();
  }
  else
  {
    //获取顶层节点
    pointcloud2 = ui->treeWidget_layer->currentItem()->parent();
  }
  //获取顶层节点第二列单元格中的QCheckBox控件
  checkBox = qobject_cast<QCheckBox*>(ui->treeWidget_layer->itemWidget(pointcloud2, 1));
  //获取顶层节点的子节点1第二列单元格中的QComboBox控件
  comboBox = qobject_cast<QComboBox*>(ui->treeWidget_layer->itemWidget(pointcloud2->child(0), 1));
  //获取顶层节点的子节点2第二列单元格中的QComboBox控件
  StyleComboBox = qobject_cast<QComboBox*>(ui->treeWidget_layer->itemWidget(pointcloud2->child(1), 1));
  //获取顶层节点的子节点3第二列单元格中的QLineEdit控件
  PointSizeLineEdit = qobject_cast<QLineEdit*>(ui->treeWidget_layer->itemWidget(pointcloud2->child(2), 1));
  if(checkBox->isChecked())
  {
    for(int index = 0; index < pointcloud2->childCount(); index++)
    {
      pointcloud2->child(index)->setDisabled(false);
    }
    comboBox->setEnabled(true);
    StyleComboBox->setEnabled(true);
    PointSizeLineEdit->setEnabled(true);
  }
  else
  {
    for(int index = 0; index < pointcloud2->childCount(); index++)
    {
      pointcloud2->child(index)->setDisabled(true);
    }
    comboBox->setEnabled(false);
    StyleComboBox->setEnabled(false);
    PointSizeLineEdit->setEnabled(false);
  }
  double doubleSize = PointSizeLineEdit->text().toDouble();
  if(doubleSize < 0.0001)
  {
    doubleSize = 0.0001;    //限制最小输入
  }
  myrviz->ModifyPointCloud2Attribute(m_LayerHash.value(pointcloud2), comboBox->currentText(), StyleComboBox->currentText(), doubleSize, checkBox->isChecked());
}


/**
 * @brief style改变槽函数
 */
void work_Obstacle::slot_onStyleComboBox_text_changed()
{
  //获取当前item
  QTreeWidgetItem* current_item = ui->treeWidget_layer->currentItem();
  //获取父节点
  QTreeWidgetItem* parent_item = current_item->parent();
  //获取当前item的索引
  int current_index = parent_item->indexOfChild(current_item);
  //获取下一个item
  QTreeWidgetItem* next_item = parent_item->child(current_index + 1);
  /*Qt 5访问TreeWidgetItem的兄弟节点比较麻烦，总体逻辑如上代码。如果您的Qt版本为Qt 6及以上，
    则可以使用QTreeWidgetItem类的sibling(int i)方法快速地访问相对位置为i（i可以为负数）兄弟节点。
    在使用指针返回值之前，建议做非空检查；这里我偷懒没有做任何非空检查*/

  //获取当前item第二列的ComboBox
  QComboBox* combobox = qobject_cast<QComboBox*>(ui->treeWidget_layer->itemWidget(current_item, 1));
  //获取下一个item第二列的LineEdit
  QLineEdit* lineedit = qobject_cast<QLineEdit*>(ui->treeWidget_layer->itemWidget(next_item, 1));
  if(combobox->currentText() == "Points")
  {
    next_item->setText(0, "大小 (Pixels)");
    lineedit->setText("3");
  }
  else
  {
    next_item->setText(0, "大小 (m)");
    lineedit->setText("0.1");
  }
}



/**
 * @brief point size编辑结束槽函数
 */
void work_Obstacle::slot_onPointSizeLineEdit_edit_finished()
{
  //获取当前item第二列的Linedit
  QLineEdit* lineedit = qobject_cast<QLineEdit*>(ui->treeWidget_layer->itemWidget(ui->treeWidget_layer->currentItem(), 1));
  double doubleSize = lineedit->text().toDouble();
  if(doubleSize < 0.0001)
  {
    doubleSize = 0.0001;      //限制最小输入
  }
  lineedit->setText(QString::number(doubleSize));
}


/**
 * @brief 修改机器人模型图层属性槽函数
 */
void work_Obstacle::slot_modify_RobotModel_attribute()
{
  QTreeWidgetItem* robotmodel;
  QCheckBox* checkBox;
  if(ui->treeWidget_layer->currentItem()->parent() == nullptr)
  {
    //获取顶层节点
    robotmodel = ui->treeWidget_layer->currentItem();
  }
  else
  {
    //获取顶层节点
    robotmodel = ui->treeWidget_layer->currentItem()->parent();
  }
  //获取顶层节点第二列单元格中的QCheckBox控件
  checkBox = qobject_cast<QCheckBox*>(ui->treeWidget_layer->itemWidget(robotmodel, 1));
  //修改属性
  myrviz->ModifyRobotModelAttribute(m_LayerHash.value(robotmodel), checkBox->isChecked());
}


/**
 * @brief 修改地图图层属性槽函数
 */
void work_Obstacle::slot_modify_Map_attribute()
{
  QTreeWidgetItem* map;
  QCheckBox* checkBox;
  QComboBox* comboBox1;
  QComboBox* comboBox2;
  if(ui->treeWidget_layer->currentItem()->parent() == nullptr)
  {
    //获取顶层节点
    map = ui->treeWidget_layer->currentItem();
  }
  else
  {
    //获取顶层节点
    map = ui->treeWidget_layer->currentItem()->parent();
  }
  //获取顶层节点第二列单元格中的QCheckBox控件
  checkBox = qobject_cast<QCheckBox*>(ui->treeWidget_layer->itemWidget(map, 1));
  //获取顶层节点的子节点1第二列单元格中的QComboBox控件
  comboBox1 = qobject_cast<QComboBox*>(ui->treeWidget_layer->itemWidget(map->child(0), 1));
  //获取顶层节点的子节点2第二列单元格中的QComboBox控件
  comboBox2 = qobject_cast<QComboBox*>(ui->treeWidget_layer->itemWidget(map->child(1), 1));
  if(checkBox->isChecked())
  {
    for(int index = 0; index < map->childCount(); index++)
    {
      map->child(index)->setDisabled(false);
    }
    comboBox1->setEnabled(true);
    comboBox2->setEnabled(true);
  }
  else
  {
    for(int index = 0; index < map->childCount(); index++)
    {
      map->child(index)->setDisabled(true);
    }
    comboBox1->setEnabled(false);
    comboBox2->setEnabled(false);
  }
  myrviz->ModifyMapAttribute(m_LayerHash.value(map), comboBox1->currentText(), comboBox2->currentText(), checkBox->isChecked());
}


/**
 * @brief 修改路径图层属性槽函数
 */
void work_Obstacle::slot_modify_Path_attribute()
{
  QTreeWidgetItem* path;
  QCheckBox* checkBox;
  QComboBox* comboBox;
  MyPalette* palette;
  if(ui->treeWidget_layer->currentItem()->parent() == nullptr)
  {
    //获取顶层节点
    path = ui->treeWidget_layer->currentItem();
  }
  else
  {
    //获取顶层节点
    path = ui->treeWidget_layer->currentItem()->parent();
  }
  //获取顶层节点第二列单元格中的QCheckBox控件
  checkBox = qobject_cast<QCheckBox*>(ui->treeWidget_layer->itemWidget(path, 1));
  //获取顶层节点的子节点1第二列单元格中的QComboBox控件
  comboBox = qobject_cast<QComboBox*>(ui->treeWidget_layer->itemWidget(path->child(0), 1));
  //获取顶层节点的子节点2第二列单元格中的MyPalette控件
  palette = qobject_cast<MyPalette*>(ui->treeWidget_layer->itemWidget(path->child(1), 1));
  if(checkBox->isChecked())
  {
    for(int index = 0; index < path->childCount(); index++)
    {
      path->child(index)->setDisabled(false);
    }
    comboBox->setEnabled(true);
    palette->setEnabled(true);
  }
  else
  {
    for(int index = 0; index < path->childCount(); index++)
    {
      path->child(index)->setDisabled(true);
    }
    comboBox->setEnabled(false);
    palette->setEnabled(false);
  }
  //调用接口
  myrviz->ModifyPathAttribute(m_LayerHash.value(path), comboBox->currentText(), palette->currentColor(), checkBox->isChecked());
}


/**
 * @brief 修改带点戳图层属性槽函数
 */
void work_Obstacle::slot_modify_PointStamped_attribute()
{
  QTreeWidgetItem* pointStamped;
  QCheckBox* checkBox;
  QComboBox* comboBox;
  MyPalette* palette;
  if(ui->treeWidget_layer->currentItem()->parent() == nullptr)
  {
    //获取顶层节点
    pointStamped = ui->treeWidget_layer->currentItem();
  }
  else
  {
    //获取顶层节点
    pointStamped = ui->treeWidget_layer->currentItem()->parent();
  }
  //获取顶层节点第二列单元格中的QCheckBox控件
  checkBox = qobject_cast<QCheckBox*>(ui->treeWidget_layer->itemWidget(pointStamped, 1));
  //获取顶层节点的子节点1第二列单元格中的QComboBox控件
  comboBox = qobject_cast<QComboBox*>(ui->treeWidget_layer->itemWidget(pointStamped->child(0), 1));
  //获取顶层节点的子节点2第二列单元格中的MyPalette控件
  palette = qobject_cast<MyPalette*>(ui->treeWidget_layer->itemWidget(pointStamped->child(1), 1));
  if(checkBox->isChecked())
  {
    for(int index = 0; index < pointStamped->childCount(); index++)
    {
      pointStamped->child(index)->setDisabled(false);
    }
    comboBox->setEnabled(true);
    palette->setEnabled(true);
  }
  else
  {
    for(int index = 0; index < pointStamped->childCount(); index++)
    {
      pointStamped->child(index)->setDisabled(true);
    }
    comboBox->setEnabled(false);
    palette->setEnabled(false);
  }
  //调用接口
  myrviz->ModifyPointStampedAttribute(m_LayerHash.value(pointStamped), comboBox->currentText(), palette->currentColor(), checkBox->isChecked());
}


/**
 * @brief 修改标记图层属性槽函数
 */
void work_Obstacle::slot_modify_Marker_attribute()
{
  QTreeWidgetItem* marker;
  QCheckBox* checkBox;
  QComboBox* comboBox;
  if(ui->treeWidget_layer->currentItem()->parent() == nullptr)
  {
    //获取顶层节点
    marker = ui->treeWidget_layer->currentItem();
  }
  else
  {
    //获取顶层节点
    marker = ui->treeWidget_layer->currentItem()->parent();
  }
  //获取顶层节点第二列单元格中的QCheckBox控件
  checkBox = qobject_cast<QCheckBox*>(ui->treeWidget_layer->itemWidget(marker, 1));
  //获取顶层节点的子节点1第二列单元格中的QComboBox控件
  comboBox = qobject_cast<QComboBox*>(ui->treeWidget_layer->itemWidget(marker->child(0), 1));
  if(checkBox->isChecked())
  {
    for(int index = 0; index < marker->childCount(); index++)
    {
      marker->child(index)->setDisabled(false);
    }
    comboBox->setEnabled(true);
  }
  else
  {
    for(int index = 0; index < marker->childCount(); index++)
    {
      marker->child(index)->setDisabled(true);
    }
    comboBox->setEnabled(false);
  }
  //调用接口
  myrviz->ModifyMarkerAttribute(m_LayerHash.value(marker), comboBox->currentText(), checkBox->isChecked());
}


/**
 * @brief 修改标记数组图层属性槽函数
 */
void work_Obstacle::slot_modify_MarkerArray_attribute()
{
  QTreeWidgetItem* markerArray;
  QCheckBox* checkBox;
  QComboBox* comboBox;
  if(ui->treeWidget_layer->currentItem()->parent() == nullptr)
  {
    //获取顶层节点
    markerArray = ui->treeWidget_layer->currentItem();
  }
  else
  {
    //获取顶层节点
    markerArray = ui->treeWidget_layer->currentItem()->parent();
  }
  //获取顶层节点第二列单元格中的QCheckBox控件
  checkBox = qobject_cast<QCheckBox*>(ui->treeWidget_layer->itemWidget(markerArray, 1));
  //获取顶层节点的子节点1第二列单元格中的QComboBox控件
  comboBox = qobject_cast<QComboBox*>(ui->treeWidget_layer->itemWidget(markerArray->child(0), 1));
  if(checkBox->isChecked())
  {
    for(int index = 0; index < markerArray->childCount(); index++)
    {
      markerArray->child(index)->setDisabled(false);
    }
    comboBox->setEnabled(true);
  }
  else
  {
    for(int index = 0; index < markerArray->childCount(); index++)
    {
      markerArray->child(index)->setDisabled(true);
    }
    comboBox->setEnabled(false);
  }
  //调用接口
  myrviz->ModifyMarkerArrayAttribute(m_LayerHash.value(markerArray), comboBox->currentText(), checkBox->isChecked());
}

/**
 * @brief 修改图像图层属性槽函数
 */
void work_Obstacle::slot_modify_Image_attribute()
{
    QTreeWidgetItem* Image;
    QCheckBox* checkBox;
    QComboBox* comboBox;
    if(ui->treeWidget_layer->currentItem()->parent() == nullptr)
    {
      //获取顶层节点
      Image = ui->treeWidget_layer->currentItem();
    }
    else
    {
      //获取顶层节点
      Image = ui->treeWidget_layer->currentItem()->parent();
    }
    //获取顶层节点第二列单元格中的QCheckBox控件
    checkBox = qobject_cast<QCheckBox*>(ui->treeWidget_layer->itemWidget(Image, 1));
    //获取顶层节点的子节点1第二列单元格中的QComboBox控件
    comboBox = qobject_cast<QComboBox*>(ui->treeWidget_layer->itemWidget(Image->child(0), 1));
    if(checkBox->isChecked())
    {
      for(int index = 0; index < Image->childCount(); index++)
      {
        Image->child(index)->setDisabled(false);
      }
      comboBox->setEnabled(true);
    }
    else
    {
      for(int index = 0; index < Image->childCount(); index++)
      {
        Image->child(index)->setDisabled(true);
      }
      comboBox->setEnabled(false);
    }
    //调用接口
    myrviz->ModifyImageAttribute(m_LayerHash.value(Image), comboBox->currentText(), checkBox->isChecked());
}

void work_Obstacle::slot_modify_Pose_attribute()
{
    QTreeWidgetItem* Pose;
    QCheckBox* checkBox;
    QComboBox* comboBox;
    if(ui->treeWidget_layer->currentItem()->parent() == nullptr)
    {
      //获取顶层节点
      Pose = ui->treeWidget_layer->currentItem();
    }
    else
    {
      //获取顶层节点
      Pose = ui->treeWidget_layer->currentItem()->parent();
    }
    //获取顶层节点第二列单元格中的QCheckBox控件
    checkBox = qobject_cast<QCheckBox*>(ui->treeWidget_layer->itemWidget(Pose, 1));
    //获取顶层节点的子节点1第二列单元格中的QComboBox控件
    comboBox = qobject_cast<QComboBox*>(ui->treeWidget_layer->itemWidget(Pose->child(0), 1));
    if(checkBox->isChecked())
    {
      for(int index = 0; index < Pose->childCount(); index++)
      {
        Pose->child(index)->setDisabled(false);
      }
      comboBox->setEnabled(true);
    }
    else
    {
      for(int index = 0; index < Pose->childCount(); index++)
      {
        Pose->child(index)->setDisabled(true);
      }
      comboBox->setEnabled(false);
    }
    //调用接口
    myrviz->ModifyPoseAttribute(m_LayerHash.value(Pose), comboBox->currentText(),QColor(20,20,20), checkBox->isChecked());
}

void work_Obstacle::slot_modify_BoundingBoxArray_attribute()
{
    QTreeWidgetItem* BoundingBoxArray;
    QCheckBox* checkBox;
    QComboBox* comboBox;
    if(ui->treeWidget_layer->currentItem()->parent() == nullptr)
    {
      //获取顶层节点
      BoundingBoxArray = ui->treeWidget_layer->currentItem();
    }
    else
    {
      //获取顶层节点
      BoundingBoxArray = ui->treeWidget_layer->currentItem()->parent();
    }
    //获取顶层节点第二列单元格中的QCheckBox控件
    checkBox = qobject_cast<QCheckBox*>(ui->treeWidget_layer->itemWidget(BoundingBoxArray, 1));
    //获取顶层节点的子节点1第二列单元格中的QComboBox控件
    comboBox = qobject_cast<QComboBox*>(ui->treeWidget_layer->itemWidget(BoundingBoxArray->child(0), 1));
    if(checkBox->isChecked())
    {
      for(int index = 0; index < BoundingBoxArray->childCount(); index++)
      {
        BoundingBoxArray->child(index)->setDisabled(false);
      }
      comboBox->setEnabled(true);
    }
    else
    {
      for(int index = 0; index < BoundingBoxArray->childCount(); index++)
      {
        BoundingBoxArray->child(index)->setDisabled(true);
      }
      comboBox->setEnabled(false);
    }
    //调用接口
    myrviz->ModifyBoundingBoxArrayAttribute(m_LayerHash.value(BoundingBoxArray), comboBox->currentText(), checkBox->isChecked());
}





void work_Obstacle::get_common_rviz_Display()
{
    //打开新增图层对话框并获得其返回值
    //qDebug() << "0520-1-3-1";
    int addLayerCode = AddLayer::getLayerInfo(this);
    //qDebug() << "0520-1-3-2";
    //获取图层显示名
    QString displayName = AddLayer::getDisplayName();
    QString displayName_Map= "地图";
    QString displayName_MarkerArray= "标记数组";
    QString displayName_dianyun2 = "点云2";
    QString displayName_BoundingBoxArray = "BoundingBoxArray";
    QString displayName_Axes= "坐标转换(TF)";
    QString displayName_Image= "图像";
    QString displayName_Path= "路径";

    //获取话题名
    QString topicName = AddLayer::getTopicName();
    qDebug() << "Topic Name in constructor: " << topicName;

//    qDebug() << "0520-1-3-3";
    add_TreeWidgetItem_Grid(ui->treeWidget_layer,displayName_Map);

    QString topicName_MarkerArray = "/lidar_track_marker";
//    qDebug() << "0520-1-3-4";
    add_TreeWidgetItem_MarkerArray(ui->treeWidget_layer,displayName_MarkerArray,topicName_MarkerArray);
//    qDebug() << "0520-1-3-5";
    QString topicName_PointCloud2 = "/velodyne_points";
    add_TreeWidgetItem_PointCloud2(ui->treeWidget_layer,displayName_dianyun2,topicName_PointCloud2);
//    qDebug() << "0520-1-3-6";
    QString topicName_Image = "/fusion_track_result";
    //add_TreeWidgetItem_Image(ui->treeWidget_layer,displayName_Image,topicName_Image);
//    qDebug() << "0520-1-3-7";
    QString topicName_BoundingBoxArray = "/lidar_track_boundingBox";
    add_TreeWidgetItem_BoundingBoxArray(ui->treeWidget_layer,displayName_BoundingBoxArray,topicName_BoundingBoxArray);
    add_TreeWidgetItem_TF(ui->treeWidget_layer,displayName_Axes);
    //qDebug() << "0520-1-3-8";
    add_TreeWidgetItem_Path(ui->treeWidget_layer,displayName_Path,topicName);
    //qDebug() << "0520-1-3-end";
    //add_TreeWidgetItem_PointCloud2(ui->treeWidget_layer,displayName_dianyun2,topicName);
}

void work_Obstacle::ReadGNSS(bool *state)
{
    if(*state == 1){ *state = 0;//重置标志位
        ui->work_pbt_GNSSConnetion->setText("GNSS读取");
    }
    else{ *state = 1;//重置标志位
        ui->work_pbt_GNSSConnetion->setText("GNSS关闭");
        //ChartItem_->clearRealPath();
    }
    rvizNode->getGNSSTransform(*state);//根据当前的state执行GNSS转换
}

void work_Obstacle::on_work_pbt_GNSSConnetion_clicked()
{
    this->ReadGNSS(&(SysSetting::instance().gnssLaunchState_));
}

void work_Obstacle::on_work_pbt_loadPath_clicked()
{
    if((SysSetting::instance().loadPathLaunchState_) == 0){ // 可以路径读取
        QString defaultDirectory = "/home/nvidia/ws/src/waypoint_loader/waypoints";
        QString csvFilePath = QFileDialog::getOpenFileName(nullptr, "选择文件", defaultDirectory, "所有文件 (*)");
        // 获取csv文件的内容，保存为QPointF点ChartItem_->CsvPointF
        //ChartItem_->CsvPointF = GlobalSignal::instance()->ReadCSV_XY(csvFilePath);
        // 将点输出到界面上(读取csv文件的点，然后写入到tractorPlanPathSeries中）
        //ChartItem_->getHistoryPoint_DotLine(ChartItem_->CsvPointF);//将路径轨迹显示出来
        // 更新ui界面控件
        ui->work_pbt_loadPath->setText("路径清除");
        ui->work_pbt_loadPath->setChecked(1);
    }else{//loadPathLaunchState_ = 1 // 路径已读
        // 清空『路径规划』路线
        //ChartItem_->tractorPlanPathSeries->clear();
        //ChartItem_->clearRealPath();// 把农机轨迹页清除
        // 更新ui界面控件
        ui->work_pbt_loadPath->setText("路径跟随");
        ui->work_pbt_loadPath->setChecked(0);
    }
    // 执行launch命令roslaunch waypoint_updater waypoint_loader_updater.launch
    rvizNode->getLoadPath(SysSetting::instance().loadPathLaunchState_);//执行launch文件
}

void work_Obstacle::on_work_pbt_AutoNavigation_clicked()
{
    if((SysSetting::instance().pathPersuitState_) == 0){ // 可以路径读取
        // 更新ui界面控件
        ui->work_pbt_AutoNavigation->setText("结束跟踪");
        ui->work_pbt_AutoNavigation->setChecked(1);
    }else{//loadPathLaunchState_ = 1 // 路径已读
        // 更新ui界面控件
        ui->work_pbt_AutoNavigation->setText("开始跟踪");
        ui->work_pbt_AutoNavigation->setChecked(0);
    }
    // 执行launch命令"roslaunch turn_on_wheeltec_robot turn_on_persuit_serial_robot.launch"
    rvizNode->autoNavigation(SysSetting::instance().pathPersuitState_);//执行launch文件
}
