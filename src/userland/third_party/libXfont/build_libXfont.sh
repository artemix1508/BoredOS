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
  "-DHAVE_CONFIG_H"
  "-I."
  "-Iinclude"
  "-Isrc/stubs"
  "-I../xorgproto/include"
  "-I../../sdk/include"
)

# Files to compile
SRCS="
src/bitmap/bdfread.c
src/bitmap/bdfutils.c
src/bitmap/bitmap.c
src/bitmap/bitmapfunc.c
src/bitmap/bitmaputil.c
src/bitmap/bitscale.c
src/bitmap/fontink.c
src/bitmap/pcfread.c
src/bitmap/pcfwrite.c
src/bitmap/snfread.c

src/builtins/dir.c
src/builtins/file.c
src/builtins/fonts.c
src/builtins/fpe.c
src/builtins/render.c

src/fontfile/bitsource.c
src/fontfile/bufio.c
src/fontfile/catalogue.c
src/fontfile/decompress.c
src/fontfile/defaults.c
src/fontfile/dirfile.c
src/fontfile/fileio.c
src/fontfile/filewr.c
src/fontfile/fontdir.c
src/fontfile/fontfile.c
src/fontfile/fontscale.c
src/fontfile/gunzip.c
src/fontfile/register.c
src/fontfile/renderers.c

src/stubs/cauthgen.c
src/stubs/csignal.c
src/stubs/delfntcid.c
src/stubs/errorf.c
src/stubs/findoldfnt.c
src/stubs/getcres.c
src/stubs/getdefptsize.c
src/stubs/getnewfntcid.c
src/stubs/gettime.c
src/stubs/initfshdl.c
src/stubs/regfpefunc.c
src/stubs/rmfshdl.c
src/stubs/servclient.c
src/stubs/setfntauth.c
src/stubs/stfntcfnt.c
src/stubs/stubsinit.c

src/util/atom.c
src/util/fontaccel.c
src/util/fontnames.c
src/util/fontutil.c
src/util/fontxlfd.c
src/util/format.c
src/util/miscutil.c
src/util/patcache.c
src/util/private.c
src/util/utilbitmap.c
"

OBJS=""
for f in $SRCS; do
    obj="${f%.c}.o"
    echo "Compiling $f..."
    $CC "${CFLAGS[@]}" -c -o "$obj" "$f"
    OBJS="$OBJS $obj"
done

echo "Archiving libXfont.a..."
x86_64-elf-ar rcs libXfont.a $OBJS
echo "Done!"
