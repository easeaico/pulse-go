cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

if(NOT APPLE)
  message(FATAL_ERROR "Can only use this toolchain on a Mac")
endif()

set(iOS_TOOLCHAIN ON)
set(CMAKE_SYSTEM_NAME visionOS)
set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_OSX_DEPLOYMENT_TARGET "1.0")
set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH NO)
set(CMAKE_IOS_INSTALL_COMBINED NO)

set(APPLE_TARGET_TRIPLE_INT arm64-apple-xros${CMAKE_OSX_DEPLOYMENT_TARGET})

# Tool Chain
# Set developer directory
execute_process(COMMAND /usr/bin/xcode-select -print-path
                OUTPUT_VARIABLE XCODE_DEVELOPER_DIR
                OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Using XCODE_DEVELOPER_DIR:" ${XCODE_DEVELOPER_DIR})
# Locate gcc
execute_process(COMMAND /usr/bin/xcrun -sdk xros -find gcc
                OUTPUT_VARIABLE CMAKE_C_COMPILER
                OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Using CMAKE_C_COMPILER:" ${CMAKE_C_COMPILER})
# Locate g++
execute_process(COMMAND /usr/bin/xcrun -sdk xros -find g++
                OUTPUT_VARIABLE CMAKE_CXX_COMPILER
                OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Using CMAKE_CXX_COMPILER:" ${CMAKE_CXX_COMPILER})
# Set the CMAKE_OSX_SYSROOT to the latest SDK found
execute_process(COMMAND /usr/bin/xcrun -sdk xros --show-sdk-path
                OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
                OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Using CMAKE_OSX_SYSROOT:" ${CMAKE_OSX_SYSROOT})

set(CMAKE_FIND_ROOT_PATH ${CMAKE_OSX_SYSROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
