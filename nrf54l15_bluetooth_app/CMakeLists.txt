#===============================================================================
# nRF54L15 蓝牙App配对数据传输项目 CMakeLists.txt
# 适用于 nRF Connect SDK v2.8.0
#===============================================================================

cmake_minimum_required(VERSION 3.20.0)

# 查找 Zephyr 包
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# 定义项目
project(nrf54l15_bluetooth_app)

# 添加源文件
target_sources(app PRIVATE 
    src/main.c
)

# 包含目录 (如果有自定义头文件)
target_include_directories(app PRIVATE
    src/
)