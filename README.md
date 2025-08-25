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

Vibration Analyzer — Hướng dẫn (Tiếng Việt)

Mục tiêu
- Hướng dẫn build, chạy và cross-compile project "VibrationAnalyzerApp" từ máy phát triển sang Raspberry Pi (Pi 4 / Pi 5 aarch64).
- Giải thích các script helper trong thư mục `scripts/` và cách khắc phục lỗi phổ biến liên quan tới Qt.

Yêu cầu cơ bản
- CMake >= 3.16
- Trình biên dịch C++ (g++/clang++), C++17
- Qt6 (Widgets). Nếu dùng QML/Charts, cần thêm các module tương ứng.
- Để cross-compile: một toolchain aarch64 (ví dụ gcc-aarch64-linux-gnu) và một sysroot của Raspberry Pi.

Nội dung chính
1) Build & chạy trên máy phát triển (Linux)

- Tạo thư mục build và build (Widgets-only mặc định):

```bash
mkdir -p build
cmake -S . -B build -DENABLE_QML=OFF
cmake --build build -j $(nproc)
```

- Chạy ứng dụng (tùy môi trường Qt):
  - Nếu Qt đã cài bằng Qt Installer (ví dụ: /home/<user>/Qt/6.x/gcc_64), dùng script wrapper để thiết lập biến môi trường:

```bash
./scripts/run_vibration.sh
```

  - Hoặc chạy thủ công (thay đường dẫn Qt nếu cần):

```bash
export QT_DIR=/home/youruser/Qt/6.x/gcc_64
export LD_LIBRARY_PATH="$QT_DIR/lib:$LD_LIBRARY_PATH"
export QT_QML_IMPORT_PATH="$QT_DIR/qml:$QT_QML_IMPORT_PATH"
export QT_PLUGIN_PATH="$QT_DIR/lib/qt6/plugins:$QT_PLUGIN_PATH"
export QT_QPA_PLATFORM=xcb
./build/VibrationAnalyzerApp
```

2) Các script helper (thư mục `scripts/`)
- `build_for_pi.sh` — Cross-build (sử dụng toolchain file `cmake/toolchain_pi_aarch64.cmake`).
  - Usage: `RPI_SYSROOT=/path/to/sysroot ./scripts/build_for_pi.sh` hoặc `./scripts/build_for_pi.sh /path/to/sysroot`.
  - Script mặc định set `-DENABLE_QML=OFF` để giảm phụ thuộc Qt.

- `install_on_pi.sh` — Script hỗ trợ cài các gói build-deps trên Raspberry Pi và build native (chạy trực tiếp trên Pi).

- `run_vibration.sh` — Wrapper để thiết lập các biến môi trường Qt và chạy binary.

3) Cross-compile cho Raspberry Pi 5 (8GB) — hướng dẫn ngắn

Chuẩn bị sysroot trên máy phát triển (khuyến nghị: tạo từ Pi thực tế)

- Trên máy phát triển (laptop), tạo thư mục sysroot và rsync dữ liệu từ Pi:

```bash
# Trên laptop
mkdir -p ~/raspi-sysroot
rsync -avz --delete --rsync-path="sudo rsync" pi@raspberrypi:/lib ~/raspi-sysroot/
rsync -avz --delete --rsync-path="sudo rsync" pi@raspberrypi:/usr ~/raspi-sysroot/
rsync -avz --delete --rsync-path="sudo rsync" pi@raspberrypi:/etc ~/raspi-sysroot/
# Nếu Qt được cài ở /opt/Qt trên Pi:
rsync -avz --delete pi@raspberrypi:/opt/Qt ~/raspi-sysroot/opt/
```

- Cài toolchain aarch64 trên máy phát triển (Debian/Ubuntu):

```bash
sudo apt update
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu rsync
```

- Cross-build (ví dụ):

```bash
# Cách A: truyền sysroot dưới dạng tham số
./scripts/build_for_pi.sh /home/user/raspi-sysroot
# Cách B: dùng biến môi trường
RPI_SYSROOT=/home/user/raspi-sysroot ./scripts/build_for_pi.sh
```

Kết quả: binary sẽ nằm trong `build-pi`.

- Copy sang Pi và chạy (trên Pi phải có Qt runtime hoặc bạn phải bundle libs):

```bash
scp build-pi/VibrationAnalyzerApp pi@raspberrypi:~/
ssh pi@raspberrypi
export QT_QPA_PLATFORM=xcb
./VibrationAnalyzerApp
```

4) Lưu ý cho Raspberry Pi 5 (8GB)
- Pi 5 dùng kiến trúc aarch64 — toolchain và sysroot phải tương ứng.
- Dùng Raspberry Pi OS 64-bit hoặc một distro 64-bit khác để tận dụng toàn bộ RAM.
- Nếu bạn muốn giao diện QML/Charts trên Pi, cài thêm module Qt tương ứng trên Pi hoặc bundle chúng với binary.

5) Khắc phục lỗi phổ biến
- "module 'QtQuick.Charts' is not installed"
  - Cài Qt Charts trên máy đích hoặc chuyển sang Widgets-only build (mặc định cross-build đã tắt QML).

- "Could not find the Qt platform plugin 'wayland'"
  - Chạy với XCB: `export QT_QPA_PLATFORM=xcb` hoặc đảm bảo `QT_PLUGIN_PATH` trỏ tới thư mục plugin trong cài Qt.

- Lỗi include/header khi dùng clangd/IDE
  - Chạy lại CMake để tạo `compile_commands.json` và paths:

```bash
rm -rf build
cmake -S . -B build
```

6) Tùy chỉnh theme
- Project đã bao gồm một stylesheet dark (`src/styles/dark.qss`) và `main.cpp` sẽ tự load `:/styles/dark.qss` nếu resource tồn tại.
- Muốn đổi theme hoặc làm selector, tôi có thể thêm cờ CLI `--theme=dark|light` hoặc file cấu hình.

7) Gợi ý tối ưu cho Pi
- Giảm tần số replot hoặc dùng circular buffer để giới hạn bộ nhớ.
- Tắt chống răng cưa (antialiasing) cho QCustomPlot.
- Giảm độ phân giải dữ liệu trước khi vẽ (downsample/adaptive sampling).

8) Muốn tôi làm gì tiếp theo?
- Tạo `scripts/package_for_pi.sh` để đóng gói binary + Qt runtime (linuxdeployqt style).
- Thêm selector theme CLI (ví dụ `--theme=dark`).
- Implement circular buffer trong `MainWindow` để giới hạn bộ nhớ.

Nếu bạn muốn tôi cập nhật README thêm chi tiết cho distro cụ thể (Debian/Ubuntu/Arch), hoặc tự động hoá rsync sysroot, nói distro và tôi sẽ bổ sung.

Hướng dẫn sử dụng (User manual)
--------------------------------

1) Mục tiêu giao diện
- Bảng điều khiển trái (COM / Log / Control): chọn cổng, bật chế độ RealTime, cấu hình ghi log, start/stop ghi.
- Phần phải: 4 đồ thị thời gian thực (2 trên, 2 dưới). Mỗi đồ thị có các tùy chọn kênh, sensitivity và sampling time phía trên.

2) Mô tả các control chính
- COM: chọn cổng kết nối cảm biến (nếu có). Mặc định có các mục ví dụ: ReCOM, COM1... (thay bằng tên thực khi kết nối).
- RealTime: nút bật/tắt thu dữ liệu thời gian thực.
- Analys Log:
  - "..." (file): mở hộp chọn file để lưu log.
  - "File Name" / "Folder name": hiển thị tên file/thư mục hiện tại.
  - "Time Log (s)": thời gian ghi mẫu (giây).
  - "Thời gian lưu (p)": thời gian lưu dữ liệu theo phút.
  - "Log Single": ghi 1 lần dữ liệu hiện tại vào file.
  - "Log Auto": bật ghi tự động theo cấu hình thời gian.
  - "Stop": dừng ghi log.
  - "Status Log": trạng thái ghi (Enable/Disable).
- Plot controls (phần trên bên phải):
  - "Plot Top/Plot Bottom": chọn kênh hiển thị cho mỗi đồ thị.
  - "Sensitivity": chọn độ nhạy (None/Low/Medium/High) — thay đổi thang hiển thị/tiền xử lý.
  - "Time Samp": thời gian mẫu (ms) cho đồ thị.
- FFT / Channel:
  - Chọn kênh để tính FFT (T/B).
  - "FFT samp (KSPS)": cấu hình sample rate cho FFT (kilo-samples-per-second).

3) Phím tắt / Hành vi window
- Ấn Esc: thoát chế độ Fullscreen (hiện hành vi: `Esc` gọi `showNormal()` trong code).
- Kéo/thu phóng đồ thị: drag/zoom (một vài tương tác cơ bản được bật).

4) Quy trình sử dụng mẫu
- A — Khởi động nhanh (local):
  1. Build và chạy `./scripts/run_vibration.sh` hoặc chạy binary trực tiếp với biến môi trường Qt.
  2. Chọn COM tương ứng.
  3. Bấm "RealTime" để bật thu dữ liệu.
  4. Chỉnh "Sensitivity" và "Time Samp" cho đồ thị trên/dưới theo nhu cầu.
  5. Nếu cần lưu dữ liệu: chọn file, cấu hình "Time Log" và bấm "Log Single" hoặc "Log Auto".

- B — Cross-compile và deploy lên Pi:
  1. Chuẩn bị sysroot và cross-build (xem mục Cross-compile phía trên).
  2. Dùng `scripts/package_for_pi.sh` để bundle binary + Qt runtime từ sysroot: 
     `./scripts/package_for_pi.sh /path/to/raspi-sysroot build-pi/VibrationAnalyzerApp dist/pi-deploy`
  3. Copy `dist/pi-deploy` lên Pi và chạy `./run.sh`.

5) Mẹo khắc phục nhanh khi đồ thị không hiện hoặc app crash
- Nếu app báo lỗi plugin platform (wayland): `export QT_QPA_PLATFORM=xcb` và chạy lại.
- Nếu app báo thiếu imports QML/Charts: build với `-DENABLE_QML=OFF` (Widgets-only) hoặc cài Qt Charts trên Pi.
- Nếu đồ thị chậm: giảm `Time Samp`, giảm replot frequency (code dùng timers ~33ms), hoặc yêu cầu tôi tối ưu thêm bằng circular buffer / replot throttling.

6) Liên hệ
- Nếu bạn muốn bản tiếng Anh hoặc chi tiết cho distro cụ thể (ví dụ Ubuntu 22.04), cho tôi biết distro/version và tôi sẽ bổ sung hướng dẫn cài Qt cụ thể.

