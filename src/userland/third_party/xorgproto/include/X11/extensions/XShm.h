#ifndef _XSHM_H_
#define _XSHM_H_

#include <X11/Xfuncproto.h>

#define X_ShmQueryVersion		0
#define X_ShmAttach			1
#define X_ShmDetach			2
#define X_ShmPutImage			3
#define X_ShmGetImage			4
#define X_ShmCreatePixmap		5

#define ShmCompletion			0
#define ShmNumEvents			1

#define ShmBadSeg			0
#define ShmNumErrors			1

typedef unsigned long ShmSeg;

typedef struct {
    ShmSeg shmseg;	/* resource id */
    int shmid;		/* kernel id */
    char *shmaddr;	/* address in client */
    Bool readOnly;	/* how the server should attach it */
} XShmSegmentInfo;

#endif
