#!/bin/bash
set -e

# Resolve repo root from env, git, or script location.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
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

CC="x86_64-elf-gcc"
CFLAGS=(
  "-Wall"
  "-Wextra"
  "-ffreestanding"
  "-O2"
  "-fno-stack-protector"
  "-fno-stack-check"
  "-fno-lto"
  "-fno-pie"
  "-m64"
  "-march=x86-64"
  "-mno-red-zone"
  "-U__INT_FAST8_TYPE__"
  "-D__INT_FAST8_TYPE__=signed char"
  "-U__UINT_FAST8_TYPE__"
  "-D__UINT_FAST8_TYPE__=unsigned char"
  "-U__INT_FAST16_TYPE__"
  "-D__INT_FAST16_TYPE__=long int"
  "-U__UINT_FAST16_TYPE__"
  "-D__UINT_FAST16_TYPE__=long unsigned int"
  "-U__INT_FAST32_TYPE__"
  "-D__INT_FAST32_TYPE__=long int"
  "-U__UINT_FAST32_TYPE__"
  "-D__UINT_FAST32_TYPE__=long unsigned int"
  "-D_GNU_SOURCE"
  "-D_DEFAULT_SOURCE"
  "-isystem" "${BOREDOS_ROOT}/src/userland/mlibc/build/mlibc-headers/usr/include"
  "-Iinclude"
  "-I../xorgproto/include"
)

SRCS="
AuDispose.c
AuFileName.c
AuGetAddr.c
AuGetBest.c
AuLock.c
AuRead.c
AuUnlock.c
AuWrite.c
"

OBJS=""
for f in $SRCS; do
    obj="${f%.c}.o"
    echo "Compiling $f..."
    $CC "${CFLAGS[@]}" -c -o "$obj" "$f"
    OBJS="$OBJS $obj"
done

echo "Archiving libXau.a..."
x86_64-elf-ar rcs libXau.a $OBJS
echo "Done!"
