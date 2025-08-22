cmake_minimum_required(VERSION 3.16)

project(VibrationAnalyzerApp LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport Quick Qml Charts QuickControls2)

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

target_link_libraries(VibrationAnalyzerApp PRIVATE Qt6::Widgets Qt6::PrintSupport Qt6::Quick Qt6::Qml Qt6::Charts Qt6::QuickControls2)
