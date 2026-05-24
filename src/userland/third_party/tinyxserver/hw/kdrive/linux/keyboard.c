#include <X11/X.h>
#include <X11/Xdefs.h>
#define XK_PUBLISHING
#include <X11/keysym.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "kdrive.h"
#include "kkeymap.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

extern int LinuxConsoleFd;
static int LinuxKbdType = 0;

static const struct {
    unsigned char scancode;
    KeySym normal;
    KeySym shifted;
} default_us_keymap[] = {
    { 1, XK_Escape, XK_Escape },
    { 2, XK_1, XK_exclam },
    { 3, XK_2, XK_at },
    { 4, XK_3, XK_numbersign },
    { 5, XK_4, XK_dollar },
    { 6, XK_5, XK_percent },
    { 7, XK_6, XK_asciicircum },
    { 8, XK_7, XK_ampersand },
    { 9, XK_8, XK_asterisk },
    { 10, XK_9, XK_parenleft },
    { 11, XK_0, XK_parenright },
    { 12, XK_minus, XK_underscore },
    { 13, XK_equal, XK_plus },
    { 14, XK_BackSpace, XK_BackSpace },
    { 15, XK_Tab, XK_Tab },
    { 16, XK_q, XK_Q },
    { 17, XK_w, XK_W },
    { 18, XK_e, XK_E },
    { 19, XK_r, XK_R },
    { 20, XK_t, XK_T },
    { 21, XK_y, XK_Y },
    { 22, XK_u, XK_U },
    { 23, XK_i, XK_I },
    { 24, XK_o, XK_O },
    { 25, XK_p, XK_P },
    { 26, XK_bracketleft, XK_braceleft },
    { 27, XK_bracketright, XK_braceright },
    { 28, XK_Return, XK_Return },
    { 29, XK_Control_L, XK_Control_L },
    { 30, XK_a, XK_A },
    { 31, XK_s, XK_S },
    { 32, XK_d, XK_D },
    { 33, XK_f, XK_F },
    { 34, XK_g, XK_G },
    { 35, XK_h, XK_H },
    { 36, XK_j, XK_J },
    { 37, XK_k, XK_K },
    { 38, XK_l, XK_L },
    { 39, XK_semicolon, XK_colon },
    { 40, XK_apostrophe, XK_quotedbl },
    { 41, XK_grave, XK_asciitilde },
    { 42, XK_Shift_L, XK_Shift_L },
    { 43, XK_backslash, XK_bar },
    { 44, XK_z, XK_Z },
    { 45, XK_x, XK_X },
    { 46, XK_c, XK_C },
    { 47, XK_v, XK_V },
    { 48, XK_b, XK_B },
    { 49, XK_n, XK_N },
    { 50, XK_m, XK_M },
    { 51, XK_comma, XK_less },
    { 52, XK_period, XK_greater },
    { 53, XK_slash, XK_question },
    { 54, XK_Shift_R, XK_Shift_R },
    { 55, XK_KP_Multiply, XK_KP_Multiply },
    { 56, XK_Alt_L, XK_Alt_L },
    { 57, XK_space, XK_space },
    { 58, XK_Caps_Lock, XK_Caps_Lock },
    { 59, XK_F1, XK_F1 },
    { 60, XK_F2, XK_F2 },
    { 61, XK_F3, XK_F3 },
    { 62, XK_F4, XK_F4 },
    { 63, XK_F5, XK_F5 },
    { 64, XK_F6, XK_F6 },
    { 65, XK_F7, XK_F7 },
    { 66, XK_F8, XK_F8 },
    { 67, XK_F9, XK_F9 },
    { 68, XK_F10, XK_F10 },
    { 69, XK_Num_Lock, XK_Num_Lock },
    { 70, XK_Scroll_Lock, XK_Scroll_Lock },
    { 71, XK_KP_Home, XK_KP_Home },
    { 72, XK_KP_Up, XK_KP_Up },
    { 73, XK_KP_Prior, XK_KP_Prior },
    { 74, XK_KP_Subtract, XK_KP_Subtract },
    { 75, XK_KP_Left, XK_KP_Left },
    { 76, XK_KP_Begin, XK_KP_Begin },
    { 77, XK_KP_Right, XK_KP_Right },
    { 78, XK_KP_Add, XK_KP_Add },
    { 79, XK_KP_End, XK_KP_End },
    { 80, XK_KP_Down, XK_KP_Down },
    { 81, XK_KP_Next, XK_KP_Next },
    { 82, XK_KP_Insert, XK_KP_Insert },
    { 83, XK_KP_Delete, XK_KP_Delete },
    { 87, XK_F11, XK_F11 },
    { 88, XK_F12, XK_F12 },
    { 96, XK_KP_Enter, XK_KP_Enter },
    { 97, XK_Control_R, XK_Control_R },
    { 98, XK_KP_Divide, XK_KP_Divide },
    { 99, XK_Print, XK_Print },
    { 100, XK_Alt_R, XK_Alt_R },
    { 102, XK_Home, XK_Home },
    { 103, XK_Up, XK_Up },
    { 104, XK_Prior, XK_Prior },
    { 105, XK_Left, XK_Left },
    { 106, XK_Right, XK_Right },
    { 107, XK_End, XK_End },
    { 108, XK_Down, XK_Down },
    { 109, XK_Next, XK_Next },
    { 110, XK_Insert, XK_Insert },
    { 111, XK_Delete, XK_Delete },
};

static void
readKernelMapping(void)
{
    KeySym *k;
    int i;

    for (i = 0; i < KD_MAX_LENGTH * KD_MAX_WIDTH; i++) {
        kdKeymap[i] = NoSymbol;
    }

    for (i = 0; i < sizeof(default_us_keymap)/sizeof(default_us_keymap[0]); i++) {
        int code = default_us_keymap[i].scancode;
        if (code >= KD_MIN_KEYCODE && code <= KD_MAX_KEYCODE) {
            k = kdKeymap + (code - KD_MIN_KEYCODE) * KD_MAX_WIDTH;
            k[0] = default_us_keymap[i].normal;
            k[1] = default_us_keymap[i].shifted;
        }
    }

    kdMinScanCode = 1;
    kdMaxScanCode = 112;
}

void
LinuxKeyboardLoad(void)
{
    readKernelMapping();
}

void
LinuxKeyboardRead(int fd, void *closure)
{
    unsigned char buf[256], *b;
    int n;

    while ((n = read(fd, buf, sizeof(buf))) > 0)
    {
        b = buf;
        while (n--)
        {
            KdEnqueueKeyboardEvent(b[0] & 0x7f, (b[0] & 0x80) != 0);
            b++;
        }
    }
}

void
LinuxKeyboardEnable(int fd, void *closure)
{
}

void
LinuxKeyboardDisable(int fd, void *closure)
{
}

int
LinuxKeyboardInit(void)
{
    if (!LinuxKbdType)
        LinuxKbdType = KdAllocInputType();

    KdRegisterFd(LinuxKbdType, LinuxConsoleFd, LinuxKeyboardRead, 0);
    return 1;
}

void
LinuxKeyboardFini(void)
{
    KdUnregisterFds(LinuxKbdType, FALSE);
}

void
LinuxKeyboardLeds(int leds)
{
}

void
LinuxKeyboardBell(int volume, int pitch, int duration)
{
}

KdKeyboardFuncs LinuxKeyboardFuncs = {
    LinuxKeyboardLoad,
    LinuxKeyboardInit,
    LinuxKeyboardLeds,
    LinuxKeyboardBell,
    LinuxKeyboardFini,
    3,
};
