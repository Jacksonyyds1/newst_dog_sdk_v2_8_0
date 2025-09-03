# Copyright (c) 2024 Dog Tracker Project
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# 设置板子配置
set(BOARD nrf54l15dk/nrf54l15/cpuapp)

set(DTC_OVERLAY_FILE ./boards/nrf54l15dk_nrf54l15_cpuapp.overlay)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(dog_tracker)

# 添加源文件
target_sources(app PRIVATE
    src/main.c
    src/ble/src/ble.c
    src/ble/src/tracker_service.c
)

# 添加头文件目录
zephyr_library_include_directories(src/include)
zephyr_library_include_directories(src/ble/include)
zephyr_library_include_directories(src/comm_mgr/include)
zephyr_library_include_directories(src/ml/include)
zephyr_library_include_directories(src/storage/include)
zephyr_library_include_directories(src/wifi/include)
zephyr_library_include_directories(src/zbus/include)

# 可选：如果要使用c_modules，取消注释下面的行
 add_subdirectory(c_modules/utils)
 add_subdirectory(c_modules/fqueue)

# 添加应用版本信息
target_compile_definitions(app PRIVATE
    APP_VERSION_MAJOR=1
    APP_VERSION_MINOR=0
    APP_VERSION_PATCH=0
)