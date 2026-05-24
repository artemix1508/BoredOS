#!/bin/sh
set -e

CC="/Users/chris/BoredOS/src/userland/third_party/zlib/cc-wrapper.sh"
CFLAGS="-Wall -Wextra -ffreestanding -O2 -fno-stack-protector -fno-stack-check -fno-lto -fno-pie -m64 -march=x86-64 -mno-red-zone -D_GNU_SOURCE -D_DEFAULT_SOURCE -isystem /Users/chris/BoredOS/src/userland/mlibc/build/mlibc-headers/usr/include"

export AR="x86_64-elf-ar"
export RANLIB="x86_64-elf-ranlib"
export CC CFLAGS

make clean || true
./configure --static --prefix=/Users/chris/BoredOS/src/userland/sdk
sed -i '' 's/^AR=.*/AR=x86_64-elf-ar/' Makefile
sed -i '' 's/^ARFLAGS=.*/ARFLAGS=rcs/' Makefile
make -j$(sysctl -n hw.ncpu) libz.a
