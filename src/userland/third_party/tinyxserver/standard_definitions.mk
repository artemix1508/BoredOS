BOREDOS_ROOT ?= $(shell cd $(dir $(lastword $(MAKEFILE_LIST)))/../../../../ && pwd)

####compiler####
CC=x86_64-elf-gcc

####compilerflags####
COMPFLAGS=-pipe -ffreestanding -O2 -fno-stack-protector -fno-stack-check -fno-lto -fno-pie -m64 -march=x86-64 -mno-red-zone \
-U__INT_FAST8_TYPE__ -D__INT_FAST8_TYPE__="signed char" \
-U__UINT_FAST8_TYPE__ -D__UINT_FAST8_TYPE__="unsigned char" \
-U__INT_FAST16_TYPE__ -D__INT_FAST16_TYPE__="long int" \
-U__UINT_FAST16_TYPE__ -D__UINT_FAST16_TYPE__="long unsigned int" \
-U__INT_FAST32_TYPE__ -D__INT_FAST32_TYPE__="long int" \
-U__UINT_FAST32_TYPE__ -D__UINT_FAST32_TYPE__="long unsigned int" \
-D_GNU_SOURCE -D_DEFAULT_SOURCE \
-isystem $(BOREDOS_ROOT)/src/userland/mlibc/build/mlibc-headers/usr/include \
-I. -I$(BOREDOS_ROOT)/src/userland/sdk/include -I$(BOREDOS_ROOT)/src/userland/third_party/xorgproto/include -I$(BOREDOS_ROOT)/src/userland/third_party/xtrans

####extensions####
DPMS=-DDPMSExtension
SCREENSAVER=-DSCREENSAVER
MIT-SHM=-DMITSHM
RENDER=-DRENDER
SHAPE=-DSHAPE
SYNC=-DXSYNC
TOG-CUP=-DTOGCUP
XCMISC=-DXCMISC
XTEST=-DXTEST
XTRAP=
XV=-DXV
RANDR=-DRANDR
XRECORD=-DXRECORD
FONTCACHE=-DNOFONTSERVERACCESS

####others####
USE_RGB_TXT=-DUSE_RGB_TXT

####where to look for fonts/colors####
FONTPATH=/Library/Fonts/
RGB=/usr/share/X11/rgb.txt

COMMONDEFS=$(COMPFLAGS) \
-DFD_SETSIZE=1024 \
-DNOERROR \
-Dlinux \
-D_POSIX_SOURCE \
-D_BSD_SOURCE \
-D_SVID_SOURCE \
-D_GNU_SOURCE \
-DX_LOCALE \
-DKDRIVESERVER \
-DGCCUSESGAS \
-DDDXOSINIT \
$(FONTCACHE) \
-DNDEBUG \
-DNARROWPROTO \
-DPIXPRIV \
$(XTEST) \
-DFUNCPROTO=15 \
-DCOMPILEDDEFAULTFONTPATH=\"${FONTPATH}\" \
-DRGB_DB=\"${RGB}\" \
-D_POSIX_C_SOURCE=2 \
$(DPMS) \
$(SYNC) \
$(SHAPE) \
$(TOG-CUP) \
$(MIT-SHM) \
$(RENDER) \
$(SCREENSAVER) \
$(SERVER_LOCK) \
$(SMART_SCHEDULE) \
$(XCMISC) \
$(XTRAP) \
$(XV) \
$(XINPUT) \
$(XKB) \
$(XKB_IN_SERVER) \
$(RANDR) \
$(XRECORD) \
$(USE_RGB_TXT) \
$(PIXPRIV) \
$(X_LOCALE) \
$(XF86BIGFONT) \
$(BIGREQS) \
-D__KERNEL_STRICT_NAMES

LDFLAGS=-Wl,-m,elf_x86_64 -nostdlib -static -no-pie -Wl,-Ttext=0x40000000 \
-Wl,--no-dynamic-linker -Wl,-z,text -Wl,-z,max-page-size=0x1000 -Wl,-z,muldefs -Wl,-e,_start \
$(BOREDOS_ROOT)/src/userland/mlibc/build/sysdeps/boredos/crt1.o \
$(BOREDOS_ROOT)/src/userland/bin/crti.o \
$(BOREDOS_ROOT)/src/userland/bin/crtn.o \
$(BOREDOS_ROOT)/src/userland/sdk/lib/libboredos.a \
$(BOREDOS_ROOT)/src/userland/sdk/lib/libXfont.a \
$(BOREDOS_ROOT)/src/userland/sdk/lib/libXau.a \
$(BOREDOS_ROOT)/src/userland/sdk/lib/libz.a \
$(BOREDOS_ROOT)/src/userland/mlibc/build/libc.a
