Vibration Analyzer — README

Mục đích
- Hướng dẫn build và chạy project trên máy khác (đã dùng Qt6). Bao gồm các biến môi trường cần thiết và khắc phục lỗi thường gặp (thiếu module QML, plugin nền).

Yêu cầu
- CMake >= 3.16, g++ (hoặc clang)
- Qt6 (widgets + qml/quick + charts + quickcontrols2)
  - Có thể dùng Qt installer (recommended) hoặc gói hệ thống của distro

Tập lệnh nhanh (build)
1. Tạo thư mục build và build:

   mkdir -p build
   cmake -S . -B build
   cmake --build build -j $(nproc)

2. Chạy bằng script helper (nếu bạn cài Qt vào /home/<user>/Qt/...):

   ./run_vibration.sh

   Script này thiết lập LD_LIBRARY_PATH, QT_QML_IMPORT_PATH, QT_PLUGIN_PATH và QT_QPA_PLATFORM=xcb trước khi chạy binary.

Nếu bạn không có script, chạy thủ công (thay đường dẫn Qt cho phù hợp):

   export QT_DIR=/home/youruser/Qt/6.9.0/gcc_64
   export LD_LIBRARY_PATH="$QT_DIR/lib:$LD_LIBRARY_PATH"
   export QT_QML_IMPORT_PATH="$QT_DIR/qml:$QT_QML_IMPORT_PATH"
   export QT_PLUGIN_PATH="$QT_DIR/lib/qt6/plugins:$QT_PLUGIN_PATH"
   export QT_QPA_PLATFORM=xcb
   ./build/VibrationAnalyzerApp

Cài đặt Qt (tùy distro)
- Dùng Qt online installer (https://www.qt.io/download) — cài Qt 6.x và chọn modules: Qt Charts, Qt Quick Controls, Qt Qml/Quick, Qt Widgets.
- Trên Debian/Ubuntu (gói có thể khác theo distro/phiên bản):

  sudo apt update
  sudo apt install build-essential cmake qt6-base-dev qt6-declarative-dev libqt6charts6 qml-module-qt6-charts qt6-quickcontrols2-plugin

  (nếu gói tên khác, tìm package tương ứng với Qt6 trên distro của bạn)

Khắc phục lỗi phổ biến
- "module 'QtQuick.Charts' is not installed"
  - Cài Qt Charts (bằng Qt Maintenance Tool hoặc apt). Hoặc loại bỏ import Charts khỏi `main.qml` (project đã có bản fallback).

- "Could not find the Qt platform plugin 'wayland'"
  - Chạy với XCB: export QT_QPA_PLATFORM=xcb
  - Đảm bảo `QT_PLUGIN_PATH` trỏ tới folder plugins trong thư mục cài Qt.

- Lỗi header không tìm thấy khi biên dịch trong IDE (clangd/VSCode):
  - Chạy lại CMake để tạo compile_commands và include paths:

    rm -rf build
    cmake -S . -B build

  - Mở workspace ở thư mục project root (nơi có CMakeLists.txt)

Chạy dưới môi trường khác (ví dụ một máy không có Qt cài sẵn)
- Tùy chọn 1: Cài Qt trên máy đó (recommended)
- Tùy chọn 2: Đóng gói binary + plugin theo Qt deployment (linuxdeployqt hoặc manual)
  - Xem tài liệu `linuxdeployqt` hoặc `windeployqt` (Windows)

Ghi chú về giao diện
- Project hỗ trợ 2 chế độ entrypoint:
  - QML: src/main.cpp dùng QQmlApplicationEngine (main.qml). Nếu dùng QML, cần QtQuick và các module QML.
  - Widgets: src/main.cpp dùng QApplication + MainWindow (Widgets + QCustomPlot). Hiện tại repository mặc định là Widgets (MainWindow) — phù hợp với đồ họa trong ảnh bạn gửi.

Muốn tôi làm gì tiếp theo?
- Tôi có thể tạo script `deploy_linux.sh` để đóng gói binary + dependencies
- Hoặc hướng dẫn chi tiết cài Qt Charts cho distro của bạn (ghi rõ tên distro/version)
- Hoặc chuyển main.qml về bản dùng Charts nếu bạn muốn cài Charts trên môi trường đích

Liên hệ nhanh
- Sau khi chạy, nếu gặp lỗi, copy/paste output terminal (toàn bộ thông báo) vào đây để tôi hướng dẫn tiếp.
