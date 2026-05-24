#include <X11/X.h>
#include <X11/Xdefs.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "kdrive.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

int LinuxConsoleFd = -1;
int LinuxApmFd = -1;
static Bool enabled = FALSE;

int
LinuxInit(void)
{
    LinuxConsoleFd = open("/dev/keyboard", O_RDWR | O_NONBLOCK, 0);
    if (LinuxConsoleFd < 0)
    {
        FatalError("LinuxInit: Cannot open /dev/keyboard (%s)\n", strerror(errno));
    }
    return 1;
}

void
LinuxEnable(void)
{
    enabled = TRUE;
}

Bool
LinuxSpecialKey(KeySym sym)
{
    return FALSE;
}

void
LinuxDisable(void)
{
    enabled = FALSE;
}

void
LinuxFini(void)
{
    if (LinuxConsoleFd >= 0)
    {
        close(LinuxConsoleFd);
        LinuxConsoleFd = -1;
    }
}

KdOsFuncs LinuxFuncs = {
    LinuxInit,
    LinuxEnable,
    LinuxSpecialKey,
    LinuxDisable,
    LinuxFini,
};

void
OsVendorInit(void)
{
    KdOsInit(&LinuxFuncs);
}
