# CMakeLists.txt for nRF54L15 Button LED Control Project

cmake_minimum_required(VERSION 3.20.0)

# 查找Zephyr包
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# 定义项目
project(button_led_control)

# 添加源文件
target_sources(app PRIVATE src/main_shell.c)