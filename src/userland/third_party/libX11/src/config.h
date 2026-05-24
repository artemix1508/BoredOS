/* config.h for libX11 1.3.5 — BoredOS cross-compile (x86_64-elf-gcc + mlibc) */
#ifndef _LIBX11_CONFIG_H_
#define _LIBX11_CONFIG_H_

#define HAVE_CONFIG_H 1

/* Standard headers available in mlibc */
#define HAVE_INTTYPES_H 1
#define HAVE_MEMORY_H   1
#define HAVE_STDINT_H   1
#define HAVE_STDLIB_H   1
#define HAVE_STRING_H   1
#define HAVE_SYS_PARAM_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_UNISTD_H   1
#define HAVE_SOCKLEN_T  1
#define STDC_HEADERS    1
#define HAVE_GETPAGESIZE 1
#define HAVE_STRTOL     1

/* Transport: Unix domain sockets ONLY */
#define UNIXCONN 1
#undef  TCPCONN
#undef  IPv6

/* No XCB — use old xtrans path */
#undef  USE_XCB

/* No locale support */
#define NO_XLOCALEDIR 1
#undef  XLOCALE
#undef  USE_DYNAMIC_LC
#undef  XLOCALEDATADIR
#undef  XLOCALEDIR
#undef  XLOCALELIBDIR
#define X_LOCALE 1

/* No thread support */
#undef  XTHREADS
#undef  XUSE_MTSAFE_API

/* No XKB */
#undef  XKB

/* No XCMS */
#undef  XCMS

/* No Secure RPC */
#undef  SECURE_RPC

/* No shared memory */
#undef  HAS_SHM

/* No dynamic module loading */
#undef  HAVE_DLFCN_H
#undef  HAVE_DLOPEN
#undef  USE_DYNAMIC_XCURSOR

/* Use poll() — mlibc supports it */
#define USE_POLL 1

/* Error DB embedded (no file path needed) */
#define XERRORDB "/Library/X11/XErrorDB"

/* Package identity */
#define PACKAGE_NAME    "libX11"
#define PACKAGE_VERSION "1.3.5"
#define PACKAGE_STRING  "libX11 1.3.5"
#define VERSION         "1.3.5"
#define PACKAGE_VERSION_MAJOR 1
#define PACKAGE_VERSION_MINOR 3
#define PACKAGE_VERSION_PATCHLEVEL 5

/* Local connection list — Unix only */
#define LOCAL_TRANSPORT_LIST "unix"

/* Data directory paths (embedded in initrd) */
#define X11_DATADIR    "/Library/X11"
#define X11_LIBDIR     "/Library/X11"
#define XKEYSYMDB      "/Library/X11/XKeysymDB"

/* GNU extensions */
#define _GNU_SOURCE 1
#define _DEFAULT_SOURCE 1

/* Signal handler return type */
#define RETSIGTYPE void

#endif /* _LIBX11_CONFIG_H_ */
