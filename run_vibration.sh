#!/usr/bin/env bash
# Wrapper to run VibrationAnalyzerApp with correct Qt environment variables
QT_DIR="/home/thanh/Qt/6.9.0/gcc_64"
export LD_LIBRARY_PATH="$QT_DIR/lib:${LD_LIBRARY_PATH}"
export QT_QML_IMPORT_PATH="$QT_DIR/qml:${QT_QML_IMPORT_PATH}"
export QT_PLUGIN_PATH="$QT_DIR/lib/qt6/plugins:${QT_PLUGIN_PATH}"
export QT_QPA_PLATFORM=xcb

# Run the built binary
exec "$(dirname "$0")/build/VibrationAnalyzerApp" "$@"
