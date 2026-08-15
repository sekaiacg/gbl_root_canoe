#!/bin/bash
set -e

NDK=${NDK_PATH}
API=31
TRIPLE="aarch64-linux-android"
OUT_DIR="build/toolkit_android"

cd "$(dirname "$0")/.."

TOOLCHAIN="$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64"
[ ! -d "$TOOLCHAIN" ] && TOOLCHAIN="$DARWIN_NDK_ROOT/toolchains/llvm/prebuilt/darwin-x86_64"

if [ ! -d "$TOOLCHAIN" ]; then
    echo "Error: NDK toolchain not found. Set NDK_PATH to your NDK path."
    exit 1
fi

CC="${TOOLCHAIN}/bin/${TRIPLE}${API}-clang"
mkdir -p "$OUT_DIR"

# NDK clang defines __ANDROID__ automatically, which selects the on-device
# (direct block-device dd) I/O path in vbmetabackup.c instead of the adb path.
echo "[1/2] Building vbmetabackup for arm64..."
$CC -O2 -o "${OUT_DIR}/vbmetabackup" vbmetabackup.c

echo "[2/2] Building vbmetaport for arm64..."
$CC -O2 -o "${OUT_DIR}/vbmetaport" vbmetaport.c

cp resources_android/run.sh "${OUT_DIR}/run.sh"
chmod +x "${OUT_DIR}/run.sh"

echo "Done. Outputs in ${OUT_DIR}/"
