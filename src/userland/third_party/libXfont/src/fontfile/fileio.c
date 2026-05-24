/*

Copyright 1991, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

*/

/*
 * Author:  Keith Packard, MIT X Consortium
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/fonts/fntfilio.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>

extern uint64_t syscall2(uint64_t sys_num, uint64_t arg1, uint64_t arg2);

static void serial_printf(const char *format, ...) {
    char buf[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    syscall2(8, 0, (uint64_t)buf);
}
#ifndef O_BINARY
#define O_BINARY O_RDONLY
#endif
#ifndef O_CLOEXEC
#define O_CLOEXEC 0
#endif

FontFilePtr
FontFileOpen (const char *name)
{
    int		fd;
    int		len;
    BufFilePtr	raw, cooked;

    serial_printf("[libXfont] FontFileOpen: trying to open %s\n", name);
    fd = open (name, O_BINARY|O_CLOEXEC);
    if (fd < 0) {
        serial_printf("[libXfont] FontFileOpen failed, errno=%d\n", errno);
	return 0;
    }
    serial_printf("[libXfont] FontFileOpen succeeded, fd=%d\n", fd);
    raw = BufFileOpenRead (fd);
    if (!raw)
    {
	close (fd);
	return 0;
    }
    len = strlen (name);
    if (len > 2 && !strcmp (name + len - 2, ".Z")) {
	cooked = BufFilePushCompressed (raw);
	if (!cooked) {
	    BufFileClose (raw, TRUE);
	    return 0;
	}
	raw = cooked;
#ifdef X_GZIP_FONT_COMPRESSION
    } else if (len > 3 && !strcmp (name + len - 3, ".gz")) {
	cooked = BufFilePushZIP (raw);
	if (!cooked) {
	    BufFileClose (raw, TRUE);
	    return 0;
	}
	raw = cooked;
#endif
#ifdef X_BZIP2_FONT_COMPRESSION
    } else if (len > 4 && !strcmp (name + len - 4, ".bz2")) {
	cooked = BufFilePushBZIP2 (raw);
	if (!cooked) {
	    BufFileClose (raw, TRUE);
	    return 0;
	}
	raw = cooked;
#endif
    }
    return (FontFilePtr) raw;
}

int
FontFileClose (FontFilePtr f)
{
    return BufFileClose ((BufFilePtr) f, TRUE);
}

