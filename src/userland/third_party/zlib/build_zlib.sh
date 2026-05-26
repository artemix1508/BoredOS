#!/bin/sh
set -e

# Resolve repo root from env, git, or script location.
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BOREDOS_ROOT="${BOREDOS_ROOT:-}"
if [ -z "$BOREDOS_ROOT" ]; then
  if command -v git >/dev/null 2>&1; then
    BOREDOS_ROOT="$(git -C "$SCRIPT_DIR" rev-parse --show-toplevel 2>/dev/null || true)"
  fi
  if [ -z "$BOREDOS_ROOT" ]; then
    BOREDOS_ROOT="$(cd "$SCRIPT_DIR/../../../../" && pwd)"
  fi
fi
if [ ! -f "$BOREDOS_ROOT/Makefile" ]; then
  echo "[ERR] Could not resolve BoredOS root (missing Makefile)." >&2
  exit 1
fi

CC="${BOREDOS_ROOT}/src/userland/third_party/zlib/cc-wrapper.sh"
CFLAGS="-Wall -Wextra -ffreestanding -O2 -fno-stack-protector -fno-stack-check -fno-lto -fno-pie -m64 -march=x86-64 -mno-red-zone -D_GNU_SOURCE -D_DEFAULT_SOURCE -isystem ${BOREDOS_ROOT}/src/userland/mlibc/build/mlibc-headers/usr/include"

export AR="x86_64-elf-ar"
export RANLIB="x86_64-elf-ranlib"
export CC CFLAGS

make clean || true
./configure --static --prefix=${BOREDOS_ROOT}/src/userland/sdk
perl -pi -e 's/^AR=.*/AR=x86_64-elf-ar/' Makefile
perl -pi -e 's/^ARFLAGS=.*/ARFLAGS=rcs/' Makefile
make -j$(sysctl -n hw.ncpu) libz.a
