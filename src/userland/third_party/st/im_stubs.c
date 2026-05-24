#include <X11/Xlib.h>
#include <stdarg.h>

// XRegisterIMInstantiateCallback stub
Bool XRegisterIMInstantiateCallback(
    Display *display,
    struct _XrmHashBucketRec *rdb,
    char *res_name,
    char *res_class,
    XIDProc callback,
    XPointer client_data)
{
    return False;
}

// XUnregisterIMInstantiateCallback stub
Bool XUnregisterIMInstantiateCallback(
    Display *display,
    struct _XrmHashBucketRec *rdb,
    char *res_name,
    char *res_class,
    XIDProc callback,
    XPointer client_data)
{
    return False;
}

// XOpenIM stub
XIM XOpenIM(
    Display *display,
    struct _XrmHashBucketRec *rdb,
    char *res_name,
    char *res_class)
{
    return NULL;
}

// XCloseIM stub
Status XCloseIM(XIM im)
{
    return Success;
}

// XSetIMValues stub
char * XSetIMValues(XIM xim, ...)
{
    return NULL;
}

// XCreateIC stub
XIC XCreateIC(XIM xim, ...)
{
    return NULL;
}

// XDestroyIC stub
void XDestroyIC(XIC ic)
{
}

// XSetICFocus stub
void XSetICFocus(XIC xic)
{
}

// XUnsetICFocus stub
void XUnsetICFocus(XIC xic)
{
}

// XSetICValues stub
char * XSetICValues(XIC xic, ...)
{
    return NULL;
}

// XVaCreateNestedList stub
XVaNestedList XVaCreateNestedList(int dummy, ...)
{
    return NULL;
}

// XmbLookupString stub
int XmbLookupString(
    XIC xic,
    XKeyPressedEvent *event,
    char *buffer_return,
    int bytes_buffer,
    KeySym *keysym_return,
    Status *status_return)
{
    if (status_return) {
        *status_return = XLookupNone;
    }
    return 0;
}
