# Toolchain file for cross-compiling to Raspberry Pi 4 (aarch64)
# Usage: pass -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain_pi_aarch64.cmake -DRPI_SYSROOT=/path/to/sysroot

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

if(NOT DEFINED RPI_SYSROOT)
  message(FATAL_ERROR "RPI_SYSROOT not defined. Provide -DRPI_SYSROOT=/path/to/sysroot or set environment variable RPI_SYSROOT")
endif()

set(CMAKE_SYSROOT "${RPI_SYSROOT}")

# Cross compilers (expect them installed on host)
set(CMAKE_C_COMPILER   aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
set(CMAKE_LINKER       aarch64-linux-gnu-ld)

# Search in sysroot for headers/libs
set(CMAKE_FIND_ROOT_PATH  "${CMAKE_SYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Do not try to run compiled programs on host
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
