# Install script for directory: D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/zephyr/subsys

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
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/canbus/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/debug/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/fb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/fs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/ipc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/logging/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/mem_mgmt/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/mgmt/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/modbus/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/pm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/portability/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/random/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/rtio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/sd/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/stats/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/storage/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/task_wdt/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/testsuite/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/tracing/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/usb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/bluetooth/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/modem/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/net/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/settings/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/shell/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/zbus/cmake_install.cmake")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/dog_tracker/build/dog_tracker/zephyr/subsys/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
