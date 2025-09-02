# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/commercial_collar")
  file(MAKE_DIRECTORY "D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/commercial_collar")
endif()
file(MAKE_DIRECTORY
  "D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/commercial_collar/build/commercial_collar"
  "D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/commercial_collar/build/_sysbuild/sysbuild/images/commercial_collar-prefix"
  "D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/commercial_collar/build/_sysbuild/sysbuild/images/commercial_collar-prefix/tmp"
  "D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/commercial_collar/build/_sysbuild/sysbuild/images/commercial_collar-prefix/src/commercial_collar-stamp"
  "D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/commercial_collar/build/_sysbuild/sysbuild/images/commercial_collar-prefix/src"
  "D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/commercial_collar/build/_sysbuild/sysbuild/images/commercial_collar-prefix/src/commercial_collar-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/commercial_collar/build/_sysbuild/sysbuild/images/commercial_collar-prefix/src/commercial_collar-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/xiao_tian_ling_v0.2/tracker_fw_sdk_V2_8_0/commercial_collar/build/_sysbuild/sysbuild/images/commercial_collar-prefix/src/commercial_collar-stamp${cfgdir}") # cfgdir has leading slash
endif()
