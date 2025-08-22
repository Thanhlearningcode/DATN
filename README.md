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

- Scripts hiện nằm trong thư mục `scripts/`.

   ./scripts/run_vibration.sh

   Script này thiết lập LD_LIBRARY_PATH, QT_QML_IMPORT_PATH, QT_PLUGIN_PATH và QT_QPA_PLATFORM=xcb trước khi chạy binary.

Nếu bạn không có script, chạy thủ công (thay đường dẫn Qt cho phù hợp):

   export QT_DIR=/home/youruser/Qt/6.9.0/gcc_64
   export LD_LIBRARY_PATH="$QT_DIR/lib:$LD_LIBRARY_PATH"
   export QT_QML_IMPORT_PATH="$QT_DIR/qml:$QT_QML_IMPORT_PATH"
   export QT_PLUGIN_PATH="$QT_DIR/lib/qt6/plugins:$QT_PLUGIN_PATH"
   export QT_QPA_PLATFORM=xcb
   ./build/VibrationAnalyzerApp

# Cross-compile & helper scripts
- Cross-build helper: `scripts/build_for_pi.sh` (requires RPI_SYSROOT set)
- Install & build on Pi helper: `scripts/install_on_pi.sh` (run on Raspberry Pi)
- Run wrapper: `scripts/run_vibration.sh`

# Usage examples
# On laptop (cross-compile)
export RPI_SYSROOT=~/raspi-sysroot
./scripts/build_for_pi.sh

# On Pi (native build)
cd project-root
./scripts/install_on_pi.sh

# Run the app via script
./scripts/run_vibration.sh

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

Cross-compile from a Linux laptop to Raspberry Pi 4 (aarch64)

This project includes helper files for cross-compiling: `cmake/toolchain_pi_aarch64.cmake` and `build_for_pi.sh`.

Quick overview (high-level steps)
1. Prepare a sysroot (copy of Pi /usr and /lib) on the laptop.
2. Install an aarch64 cross-compiler on the laptop.
3. Run the provided `build_for_pi.sh` (set RPI_SYSROOT) to cross-build a Release binary.
4. Copy the binary to the Pi and run (ensure Qt runtime on Pi or bundle libs).

Detailed commands (example)

# On laptop: install tools
sudo apt update
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu rsync

# Create a directory for the Pi sysroot on laptop
mkdir -p ~/raspi-sysroot

# Rsync the Pi root libraries to laptop (replace user@pi)
rsync -a --delete --rsync-path="sudo rsync" user@pi:/lib ~/raspi-sysroot/
rsync -a --delete --rsync-path="sudo rsync" user@pi:/usr ~/raspi-sysroot/
# If Qt is installed on Pi under /opt/Qt copy it too:
rsync -a --delete user@pi:/opt/Qt ~/raspi-sysroot/opt/

# On laptop: set RPI_SYSROOT and run the helper script (project root)
export RPI_SYSROOT=~/raspi-sysroot
./build_for_pi.sh

The script will call CMake with `-DCMAKE_TOOLCHAIN_FILE=cmake/toolchain_pi_aarch64.cmake` and build to `build-pi`.

After build

# Copy binary to Pi (replace user@pi)
scp build-pi/VibrationAnalyzerApp user@pi:/home/user/

# On Pi: run (ensure Qt runtime exists on Pi or set LD_LIBRARY_PATH to Qt libs)
export QT_QPA_PLATFORM=xcb
./VibrationAnalyzerApp

Notes and tips
- For Raspberry Pi 4 (4GB) we recommend Raspberry Pi OS 64-bit to use all memory.
- Recommended to set `-DENABLE_QML=OFF` for cross-build to reduce Qt dependencies (Widgets-only build). The build script already uses that flag.
- If Pi doesn't have Qt runtime, either install Qt on Pi (apt or Qt installer) or copy needed Qt libraries and plugins from the sysroot (or use linuxdeployqt on the Pi to bundle).
- After building, strip the binary to reduce size: `strip build-pi/VibrationAnalyzerApp`.
- If you need to enable QML on Pi, ensure the same Qt version and modules (Quick, Qml, Charts, QuickControls2) are present on the Pi.

If you want, I can also:
- Add a script to automatically rsync a minimal sysroot (only required Qt folders + /usr/lib) from the Pi. 
- Add a packaging step (linuxdeployqt) to bundle Qt libs on the Pi.
