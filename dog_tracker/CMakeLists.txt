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
 add_subdirectory(c_modules/json)        # 添加JSON模块
 add_subdirectory(c_modules/imu)         # 添加IMU模块  
 add_subdirectory(c_modules/modem)       # 添加调制解调器模块
 add_subdirectory(c_modules/pmic)       # 添加PMIC模块
 add_subdirectory(c_modules/uicr)       # 添加UICR模块
 add_subdirectory(c_modules/tde)        # 添加TDE模块
 add_subdirectory(c_modules/watchdog)   # 添加看门狗模块
# 添加src子模块
add_subdirectory(src/comm_mgr)          # 添加通信管理器模块
add_subdirectory(src/ml)                # 添加机器学习模块
add_subdirectory(src/storage)           # 添加存储模块
add_subdirectory(src/wifi)              # 添加WiFi模块
add_subdirectory(src/zbus)              # 添加ZBus模块

# 添加应用版本信息
target_compile_definitions(app PRIVATE
    APP_VERSION_MAJOR=1
    APP_VERSION_MINOR=0
    APP_VERSION_PATCH=0
)