#include <X11/X.h>
#include <X11/Xdefs.h>
#include <X11/Xmd.h>
#include <stdlib.h>
#include "agp.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

Bool KdAgpGARTSupported(void) 
{ 
    return FALSE; 
}

AgpInfoPtr KdGetAGPInfo(int screenNum) 
{ 
    return NULL; 
}

Bool KdAcquireGART(int screenNum) 
{ 
    return FALSE; 
}

Bool KdReleaseGART(int screenNum) 
{ 
    return FALSE; 
}

int KdAllocateGARTMemory(int screenNum, unsigned long size, int type, unsigned long *physical) 
{ 
    return -1; 
}

Bool KdBindGARTMemory(int screenNum, int key, unsigned long offset) 
{ 
    return FALSE; 
}

Bool KdUnbindGARTMemory(int screenNum, int key) 
{ 
    return FALSE; 
}

Bool KdEnableAGP(int screenNum, CARD32 mode) 
{ 
    return FALSE; 
}
