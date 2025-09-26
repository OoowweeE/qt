##############################################################################
# CMake
##############################################################################
#
cmake_minimum_required(VERSION 3.16)
#项目名称
project(ros_qt_demo)

#add
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DEBUG_POSTFIX "d")
##############################################################################
# Catkin
##############################################################################

# qt_build provides the qt cmake glue, roscpp the comms for a default talker
find_package(catkin REQUIRED COMPONENTS rviz roscpp sensor_msgs std_msgs jsk_recognition_msgs image_transport cv_bridge)
find_package(Qt5 COMPONENTS Charts       REQUIRED)
find_package(Qt5 COMPONENTS SerialPort  REQUIRED)
find_package(Qt5 COMPONENTS VirtualKeyboard  REQUIRED)


find_package(Qt5 COMPONENTS Widgets WebEngineWidgets WebChannel REQUIRED)
find_package(Qt5 COMPONENTS Sql REQUIRED)
#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Sql REQUIRED)
#find_package(catkin REQUIRED COMPONENTS roscpp)
include_directories(${catkin_INCLUDE_DIRS})
#add
# 查找 Qt5 核心模块

# 包含 Qt 核心模块和 SVG 模块
find_package(Qt5 REQUIRED Core Widgets Svg Xml)
set(QT_LIBRARIES Qt5::Widgets)
# Use this to define what the package will export (e.g. libs, headers).
# Since the default here is to produce only a binary, we don't worry about
# exporting anything.
catkin_package()


##############################################################################
 #第三方库：QxOrm
#############################################################################
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/QxOrm)
#导入头文件目录
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/QxOrm/include)
#导入库文件目录
#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/QxOrm/lib)
#添加_BUILDING_USER预处理
#add_definitions(-D_BUILDING_USER)
########################################不兼容放弃#############################################
##*****#############################################
# set(Xlnt_DIR "/usr/local/lib/cmake/xlnt")
# find_package(Xlnt REQUIRED)
# set(xlnt_lib "/usr/local/lib/libxlnt.so")
##*****#############################################

##*****#############################################
#第三方库：rapidcsv

set(rapidCsv_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/rapidcsv")
add_subdirectory(${rapidCsv_DIR})

##*****#############################################

##############################################################################
# Qt Environment
##############################################################################

# this comes from qt_build's qt-ros.cmake which is automatically
# included via the dependency call in package.xml
#remove
#rosbuild_prepare_qt4(QtCore QtGui) # Add the appropriate components to the component list here
########################
# 预编译头文件
########################
#set(PRECOMPILED_HEADERS "include/ros_qt_demo/dataBase/precompiled.h")
#set(PRECOMPILED_CPP "src/precompiled.cpp")
# add_executable(ros_qt_demo main.cpp ${PRECOMPILED_CPP}) 统一在下面添加了，不然会报错，找add_executable
# target_compile_options(ros_qt_demo PRIVATE -Winvalid-pch)
##############################################################################
# Sections
##############################################################################

# 添加源文件：自动索引ui文件、资源文件、头文件和源码文件
file(GLOB QT_FORMS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ui/*.ui)
file(GLOB QT_RESOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} resources/*.qrc)
file(GLOB_RECURSE QT_MOC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS include/*.hpp *.h)
file(GLOB_RECURSE HEADERS "include/*.hpp" "include/*.h")
file(GLOB_RECURSE QT_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS src/*.cpp)


#change
QT5_ADD_RESOURCES(QT_RESOURCES_CPP ${QT_RESOURCES})
QT5_WRAP_UI(QT_FORMS_HPP ${QT_FORMS})
QT5_WRAP_CPP(QT_MOC_HPP ${QT_MOC})
##############################################################################
# Sources
##############################################################################


# 添加包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/nmea_navsat_driver
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model
)

# 定义目标
add_executable(ros_qt_demo ${QT_SOURCES} ${QT_RESOURCES_CPP} ${QT_FORMS_HPP} ${QT_MOC_HPP} ${HEADERS} )

# # 为目标添加include路径: 将特定目录下的头文件加入到目标中
target_include_directories(ros_qt_demo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/model
    ${CMAKE_CURRENT_SOURCE_DIR}/include/common
    ${CMAKE_CURRENT_SOURCE_DIR}/include/view
    ${CMAKE_CURRENT_SOURCE_DIR}/include/controller
    ${CMAKE_CURRENT_SOURCE_DIR}/include/nmea_navsat_driver
)
message(STATUS "Include directories--------------------: ${CMAKE_CURRENT_SOURCE_DIR}/include")


##############################################################################
# Binaries
##############################################################################


target_link_libraries(ros_qt_demo
    ${QT_LIBRARIES}
    ${catkin_LIBRARIES}
    Qt5::Svg
    Qt5::Xml
    rapidcsv
    Qt5::SerialPort
    Qt5::Charts
    Qt5::WebEngineWidgets
    Qt5::WebChannel
    #Qt${QT_VERSION_MAJOR}::Sql
    Qt5::Sql
    Qt5::VirtualKeyboard
    )

install(TARGETS ros_qt_demo RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
#target_precompile_headers(ros_qt_demo PRIVATE include/ros_qt_demo/dataBase/precompiled.h)
#target_compile_options(ros_qt_demo PRIVATE -include include/ros_qt_demo/dataBase/precompiled.h)
