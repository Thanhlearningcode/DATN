cmake_minimum_required(VERSION 3.16)

project(VibrationAnalyzerApp LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_QML "Enable QML/Quick features" ON)

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
    src/qcustomplot.cpp
    resources.qrc
)

set(HEADERS
    inc/MainWindow.h
    inc/Simulation.h
    inc/qcustomplot.h
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
