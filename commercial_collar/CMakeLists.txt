# Copyright (c) 2024, Nestle Purina Pet Care. All rights reserved
# SPDX-License-Identifier: LicenseRef-Proprietary

cmake_minimum_required(VERSION 3.20.0)

# 设置 overlay 文件 - 必须在 find_package(Zephyr) 之前
set(DTC_OVERLAY_FILE ./nrf54l15_nrf54l15.overlay)

add_compile_options(-Werror)

get_filename_component(APPLICATION_PROJECT_DIR
    ${CMAKE_CURRENT_LIST_DIR}/..
    ABSOLUTE
)

list(APPEND CONF_FILE prj.conf)

if (EXISTS prj.local.conf)
    list(APPEND CONF_FILE prj.local.conf)
endif()

if (CONFIG_RELEASE_BUILD)
    message("Doing a RELEASE build")
    list(APPEND CONF_FILE prj.release.conf)
endif()

message("CONF_FILE=${CONF_FILE}")
list(APPEND BOARD_ROOT ${APPLICATION_PROJECT_DIR})
list(APPEND DTS_ROOT ${APPLICATION_PROJECT_DIR})
message("ADDING BOARD_ROOT=${BOARD_ROOT}")
message("ADDING DTS_ROOT=${DTS_ROOT}")

list(APPEND EXTRA_ZEPHYR_MODULES ${APPLICATION_PROJECT_DIR}/lib/drivers)
message("MODULES=${EXTRA_ZEPHYR_MODULES}")

# 必须在设置完所有配置后调用
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# 项目信息
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

project(commercial_collar VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

# 版本定义
add_definitions(-DAPP_VERSION_MAJOR_CMAKE=${PROJECT_VERSION_MAJOR})
add_definitions(-DAPP_VERSION_MINOR_CMAKE=${PROJECT_VERSION_MINOR})
add_definitions(-DAPP_VERSION_PATCH_CMAKE=${PROJECT_VERSION_PATCH})

# Git 信息
execute_process(
  COMMAND git rev-parse --short=6 HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)

# 检查仓库是否有未提交的修改
execute_process(
  COMMAND git status --porcelain -uno
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_DIRTY_STATUS
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)

# 如果有未提交的修改，添加 '-dirty' 后缀
if(NOT GIT_DIRTY_STATUS STREQUAL "")
  set(GIT_HASH "${GIT_HASH}-dirty")
endif()

if(GIT_HASH)
  add_definitions(-DGIT_HASH_CMAKE=\"${GIT_HASH}\")
else()
  add_definitions(-DGIT_HASH_CMAKE=\"unknown\")
endif()

# 构建机器名
execute_process(
  COMMAND hostname
  OUTPUT_VARIABLE BUILD_MACHINE
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)

# 构建日期
string(TIMESTAMP BUILD_DATE "%Y-%m-%d %H:%M:%S")

# 传递变量到代码
if(BUILD_MACHINE)
  add_definitions(-DBUILD_MACHINE_CMAKE=\"${BUILD_MACHINE}\")
else()
  add_definitions(-DBUILD_MACHINE_CMAKE=\"unknown\")
endif()
add_definitions(-DBUILD_DATE_CMAKE=\"${BUILD_DATE}\")

# 添加源文件
target_sources(app PRIVATE
  src/main.c
  # 逐步添加其他源文件
  # src/wifi/src/wifi.c
  # src/wifi/src/wifi_at.c
  # c_modules/json/src/d1_json.c
)

# 包含目录
target_include_directories(app PRIVATE
  src/
  # c_modules/json/include
  # c_modules/pmic/include
  # src/wifi/include
)