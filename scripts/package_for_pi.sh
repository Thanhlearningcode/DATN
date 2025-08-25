#!/usr/bin/env bash
# Bundle a cross-built binary and Qt runtime from a Pi sysroot into a deploy folder.
# Usage:
#   ./scripts/package_for_pi.sh /path/to/raspi-sysroot [path/to/binary] [outdir]
# Example:
#   ./scripts/package_for_pi.sh ~/raspi-sysroot build-pi/VibrationAnalyzerApp dist/pi-deploy

set -euo pipefail
SYSROOT=${1:-}
BINARY=${2:-build-pi/VibrationAnalyzerApp}
OUTDIR=${3:-dist/pi-deploy}

if [ -z "$SYSROOT" ]; then
  echo "Usage: $0 /path/to/raspi-sysroot [path/to/binary] [outdir]" >&2
  exit 1
fi

if [ ! -d "$SYSROOT" ]; then
  echo "Sysroot not found: $SYSROOT" >&2
  exit 1
fi

if [ ! -f "$BINARY" ]; then
  echo "Binary not found: $BINARY" >&2
  exit 1
fi

rm -rf "$OUTDIR"
mkdir -p "$OUTDIR/bin" "$OUTDIR/qt" "$OUTDIR/logs"

echo "Copying binary to deploy directory..."
cp "$BINARY" "$OUTDIR/bin/"
chmod +x "$OUTDIR/bin/$(basename "$BINARY")"

# Helper to try copying common Qt locations from sysroot
rsync_opts=( -a --delete --prune-empty-dirs )

copy_if_exists() {
  local src="$1"
  local dst="$2"
  if [ -e "$src" ]; then
    echo "Copying $src -> $dst"
    mkdir -p "$(dirname "$dst")"
    rsync "${rsync_opts[@]}" "$src" "$dst"
    return 0
  fi
  return 1
}

# Common Qt install locations on Pi/sysroot
# Prefer an /opt/Qt copy if present (e.g., Qt installer), otherwise try distro paths
if copy_if_exists "$SYSROOT/opt/Qt/" "$OUTDIR/qt/"; then
  echo "Copied /opt/Qt from sysroot"
else
  # Try distro-style locations
  copy_if_exists "$SYSROOT/usr/lib/qt6/" "$OUTDIR/qt/lib/" || true
  copy_if_exists "$SYSROOT/usr/lib/aarch64-linux-gnu/qt6/" "$OUTDIR/qt/lib/" || true
  copy_if_exists "$SYSROOT/usr/lib/" "$OUTDIR/qt/lib/" || true
  # Also copy plugins (platforms, imageformats, printsupport, platformsthemes)
  copy_if_exists "$SYSROOT/usr/lib/qt6/plugins/" "$OUTDIR/qt/plugins/" || true
  copy_if_exists "$SYSROOT/usr/lib/aarch64-linux-gnu/qt6/plugins/" "$OUTDIR/qt/plugins/" || true
fi

# Some distros place Qt libs under lib/arch/qt6 or similar; try a few fallbacks
copy_if_exists "$SYSROOT/lib/" "$OUTDIR/qt/lib/" || true

# Create a small run wrapper
cat > "$OUTDIR/run.sh" <<'EOF'
#!/usr/bin/env bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="$DIR/qt/lib:$LD_LIBRARY_PATH"
export QT_PLUGIN_PATH="$DIR/qt/plugins:$QT_PLUGIN_PATH"
export QT_QML_IMPORT_PATH="$DIR/qt/qml:$QT_QML_IMPORT_PATH"
export QT_QPA_PLATFORM=xcb
exec "$DIR/bin/$(basename "$BINARY")" "$@"
EOF

chmod +x "$OUTDIR/run.sh"
chmod -R a+rX "$OUTDIR"

cat > "$OUTDIR/README-pack.txt" <<EOF
Run instructions (on Raspberry Pi):
1) Copy the whole directory to the Pi (scp/rsync). Example:
   scp -r $OUTDIR pi@raspberrypi:~/vibration-deploy
2) On the Pi:
   cd ~/vibration-deploy
   ./run.sh

Notes:
- This script copies Qt files from the provided sysroot. It may include extra files; you can trim plugins or libs you don't need.
- If the app still fails to find plugins, set QT_PLUGIN_PATH to point to the correct folder on the Pi or install Qt packages on the Pi.
- You can use linuxdeployqt on the Pi to finalize the bundle if available.
EOF

echo "Packaging complete. Deploy folder: $OUTDIR"
