#!/usr/bin/env bash
QT_DIR="/home/thanh/Qt/6.9.0/gcc_64"
export LD_LIBRARY_PATH="$QT_DIR/lib:${LD_LIBRARY_PATH:-}"
export QT_QML_IMPORT_PATH="$QT_DIR/qml:${QT_QML_IMPORT_PATH:-}"
export QT_PLUGIN_PATH="$QT_DIR/lib/qt6/plugins:${QT_PLUGIN_PATH:-}"
export QT_QPA_PLATFORM=xcb

exec "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../build/VibrationAnalyzerApp" "$@"