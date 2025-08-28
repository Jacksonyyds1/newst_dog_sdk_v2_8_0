# Install script for directory: D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/zephyr

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Zephyr-Kernel")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "MinSizeRel")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "D:/Dog_track/zephyr-sdk-0.16.9_windows-x86_64/zephyr-sdk-0.16.9/arm-zephyr-eabi/bin/arm-zephyr-eabi-objdump.exe")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/zephyr/arch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/zephyr/lib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/zephyr/soc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/zephyr/boards/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/zephyr/subsys/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/zephyr/drivers/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/nrf/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/mcuboot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/mbedtls/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/trusted-firmware-m/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/cjson/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/azure-sdk-for-c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/cirrus-logic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/openthread/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/suit-processor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/memfault-firmware-sdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/canopennode/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/chre/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/lz4/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/nanopb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/zscilib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/cmsis/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/cmsis-dsp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/cmsis-nn/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/fatfs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/hal_nordic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/hal_st/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/hal_wurthelektronik/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/hostap/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/libmetal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/liblc3/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/littlefs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/loramac-node/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/lvgl/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/mipi-sys-t/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/nrf_hw_models/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/open-amp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/picolibc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/segger/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/tinycrypt/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/uoscore-uedhoc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/zcbor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/nrfxlib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/modules/connectedhomeip/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/zephyr/kernel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/zephyr/cmake/flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/zephyr/cmake/usage/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/zephyr/cmake/reports/cmake_install.cmake")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/nrf54l15_bluetooth_app/build/nrf54l15_bluetooth_app/zephyr/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
