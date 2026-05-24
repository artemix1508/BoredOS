#!/bin/bash
set -e

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
  "-isystem" "/Users/chris/BoredOS/src/userland/mlibc/build/mlibc-headers/usr/include"
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
