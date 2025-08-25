cmake_minimum_required(VERSION 3.16)

project(VibrationAnalyzerApp LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_QML "Enable QML/Quick features" ON)
option(ENABLE_NIDAQ "Enable NI-DAQmx support" OFF)

if(ENABLE_NIDAQ)
  set(NIDAQMX_ROOT "" CACHE PATH "NI-DAQmx root folder (contains include/ and lib/)")
  if(NOT NIDAQMX_ROOT)
    message(FATAL_ERROR "Set NIDAQMX_ROOT to NI installation folder (contains include/NIDAQmx.h) when ENABLE_NIDAQ=ON")
  endif()
  include_directories(${NIDAQMX_ROOT}/include)
  find_library(NIDAQMX_LIB NAMES NIDAQmx HINTS ${NIDAQMX_ROOT}/lib ${NIDAQMX_ROOT}/bin)
  if(NOT NIDAQMX_LIB)
    message(FATAL_ERROR "NIDAQmx library not found in ${NIDAQMX_ROOT}")
  endif()
  add_compile_definitions(NIDAQ_AVAILABLE=1)
endif()

if(DEFINED CMAKE_SYSROOT)
  # When cross-compiling, try to prefer Qt installed in sysroot
  set(Qt6_DIR "${CMAKE_SYSROOT}/usr/lib/cmake/Qt6" CACHE PATH "Qt6 dir in sysroot")
endif()

if(NOT ENABLE_QML)
  message(STATUS "QML disabled; building Widgets-only binary")
  find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport)
else()
  find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport Quick Qml Charts QuickControls2)
endif()

set(SOURCES
  src/main.cpp
  src/MainWindow.cpp
  src/Simulation.cpp
  src/DAQWorker.cpp
  src/AppController.cpp
  src/qcustomplot.cpp
  resources.qrc
)

set(HEADERS
  inc/MainWindow.h
  inc/Simulation.h
  inc/qcustomplot.h
  inc/AppController.h
)

add_executable(VibrationAnalyzerApp
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(VibrationAnalyzerApp PRIVATE inc)

if(NOT ENABLE_QML)
  target_link_libraries(VibrationAnalyzerApp PRIVATE Qt6::Widgets Qt6::PrintSupport)
else()
  target_link_libraries(VibrationAnalyzerApp PRIVATE Qt6::Widgets Qt6::PrintSupport Qt6::Quick Qt6::Qml Qt6::Charts Qt6::QuickControls2)
endif()

if(ENABLE_NIDAQ)
  target_link_libraries(VibrationAnalyzerApp PRIVATE ${NIDAQMX_LIB})
endif()
