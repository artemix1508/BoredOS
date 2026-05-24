#ifndef XFT_COMPAT_H
#define XFT_COMPAT_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <string.h>

// Types
typedef struct {
    Display *dpy;
    Drawable drawable;
    GC gc;
    Visual *visual;
    Colormap cmap;
} XftDraw;

typedef struct {
    unsigned short red;
    unsigned short green;
    unsigned short blue;
    unsigned short alpha;
} XRenderColor;

typedef struct {
    unsigned long pixel;
    XRenderColor color;
} XftColor;

typedef struct {
    XFontStruct *match;
    int ascent;
    int descent;
    short max_bounds_width;
    short max_advance_width;
    void *pattern; // stubbed FcPattern
} XftFont;

typedef struct {
    XftFont *font;
    short x, y;
    unsigned int glyph;
} XftGlyphFontSpec;

typedef struct {
    unsigned short width;
    unsigned short height;
    short x;
    short y;
    short xOff;
    short yOff;
} XGlyphInfo;

typedef unsigned char FcChar8;
typedef unsigned int FcChar32;
typedef unsigned int FT_UInt;

typedef void * FcPattern;
typedef struct { int nfont; } FcFontSet;
typedef int FcResult;
typedef void * FcCharSet;

#define FcMatchPattern 1
#define FcResultMatch 1
#define XftResultMatch 1
#define FC_SLANT "slant"
#define FC_SLANT_ITALIC 100
#define FC_SLANT_ROMAN 0
#define FC_WEIGHT "weight"
#define FC_WEIGHT_BOLD 200
#define FC_PIXEL_SIZE "pixelsize"
#define FC_SIZE "size"
#define FC_CHARSET "charset"
#define FC_SCALABLE "scalable"

// Functions
static inline XftDraw *XftDrawCreate(Display *dpy, Drawable drawable, Visual *visual, Colormap cmap) {
    XftDraw *draw = malloc(sizeof(XftDraw));
    draw->dpy = dpy;
    draw->drawable = drawable;
    draw->gc = XCreateGC(dpy, drawable, 0, NULL);
    draw->visual = visual;
    draw->cmap = cmap;
    return draw;
}

static inline void XftDrawChange(XftDraw *draw, Drawable drawable) {
    if (draw) draw->drawable = drawable;
}

static inline void XftDrawDestroy(XftDraw *draw) {
    if (draw) {
        XFreeGC(draw->dpy, draw->gc);
        free(draw);
    }
}

static inline Bool XftColorAllocName(Display *dpy, Visual *visual, Colormap cmap, const char *name, XftColor *color) {
    XColor xcolor, exact;
    if (XAllocNamedColor(dpy, cmap, name, &xcolor, &exact)) {
        color->pixel = xcolor.pixel;
        color->color.red = xcolor.red;
        color->color.green = xcolor.green;
        color->color.blue = xcolor.blue;
        color->color.alpha = 0xffff;
        return True;
    }
    return False;
}

static inline Bool XftColorAllocValue(Display *dpy, Visual *visual, Colormap cmap, const XRenderColor *color, XftColor *out_color) {
    XColor xcolor;
    xcolor.red = color->red;
    xcolor.green = color->green;
    xcolor.blue = color->blue;
    xcolor.flags = DoRed | DoGreen | DoBlue;
    if (XAllocColor(dpy, cmap, &xcolor)) {
        out_color->pixel = xcolor.pixel;
        out_color->color = *color;
        return True;
    }
    return False;
}

static inline void XftColorFree(Display *dpy, Visual *visual, Colormap cmap, XftColor *color) {
    XFreeColors(dpy, cmap, &color->pixel, 1, 0);
}

static inline void XftDrawRect(XftDraw *draw, const XftColor *color, int x, int y, unsigned int width, unsigned int height) {
    XSetForeground(draw->dpy, draw->gc, color->pixel);
    XFillRectangle(draw->dpy, draw->drawable, draw->gc, x, y, width, height);
}

static inline void XftDrawSetClipRectangles(XftDraw *draw, int xOrigin, int yOrigin, const XRectangle *rects, int n) {
    XSetClipRectangles(draw->dpy, draw->gc, xOrigin, yOrigin, rects, n, Unsorted);
}

static inline void XftDrawSetClip(XftDraw *draw, void *clip) {
    if (draw) {
        XSetClipMask(draw->dpy, draw->gc, None);
    }
}

static inline void * XftXlfdParse(const char *xlfd, Bool ignore1, Bool ignore2) {
    return strdup(xlfd);
}

static inline XftFont *XftFontOpenPattern(Display *dpy, void *pattern) {
    XftFont *font = malloc(sizeof(XftFont));
    font->pattern = pattern;
    font->match = XLoadQueryFont(dpy, pattern ? (const char *)pattern : "fixed");
    if (!font->match) {
        font->match = XLoadQueryFont(dpy, "fixed");
    }
    if (font->match) {
        font->ascent = font->match->ascent;
        font->descent = font->match->descent;
        font->max_bounds_width = font->match->max_bounds.width;
        font->max_advance_width = font->match->max_bounds.width;
    } else {
        free(font);
        return NULL;
    }
    return font;
}

static inline void XftFontClose(Display *dpy, XftFont *font) {
    if (font) {
        if (font->match) XFreeFont(dpy, font->match);
        free(font);
    }
}

static inline void XftTextExtentsUtf8(Display *dpy, XftFont *font, const FcChar8 *string, int len, XGlyphInfo *extents) {
    int width = XTextWidth(font->match, (const char *)string, len);
    extents->xOff = width;
    extents->width = width;
    extents->height = font->ascent + font->descent;
    extents->x = 0;
    extents->y = -font->ascent;
}

static inline void XftDrawGlyphFontSpec(XftDraw *draw, const XftColor *color, const XftGlyphFontSpec *specs, int len) {
    XSetForeground(draw->dpy, draw->gc, color->pixel);
    for (int i = 0; i < len; i++) {
        XSetFont(draw->dpy, draw->gc, specs[i].font->match->fid);
        char c = (char)specs[i].glyph;
        XDrawString(draw->dpy, draw->drawable, draw->gc, specs[i].x, specs[i].y, &c, 1);
    }
}

static inline unsigned int XftCharIndex(Display *dpy, XftFont *font, FcChar32 rune) {
    return (unsigned int)rune;
}

static inline void XftDefaultSubstitute(Display *dpy, int screen, void *pattern) {}
static inline int XftPatternGetInteger(void *pattern, const char *object, int id, int *i) {
    return 0;
}

// Fc Fontconfig Stubs
static inline int FcInit(void) { return 1; }
static inline void *FcPatternDuplicate(void *p) { return p ? strdup((const char *)p) : NULL; }
static inline void FcConfigSubstitute(void *config, void *p, int kind) {}
static inline void *FcFontMatch(void *config, void *p, int *res) {
    *res = FcResultMatch;
    return p ? strdup((const char *)p) : NULL;
}
static inline void FcPatternDestroy(void *p) { if (p) free(p); }
static inline void FcFontSetDestroy(FcFontSet *s) { if (s) free(s); }
static inline void FcCharSetDestroy(FcCharSet cs) {}
static inline void *FcNameParse(const FcChar8 *name) { return name ? strdup((const char *)name) : NULL; }
static inline void FcPatternDel(void *p, const char *object) {}
static inline void FcPatternAddDouble(void *p, const char *object, double d) {}
static inline int FcPatternGetDouble(void *p, const char *object, int id, double *d) {
    *d = 12.0;
    return FcResultMatch;
}
static inline void FcPatternAddInteger(void *p, const char *object, int i) {}
static inline void *FcCharSetCreate(void) { return (void *)1; }
static inline void FcCharSetAddChar(void *cs, FcChar32 rune) {}
static inline void FcPatternAddCharSet(void *p, const char *object, void *cs) {}
static inline void FcPatternAddBool(void *p, const char *object, Bool b) {}
static inline void FcDefaultSubstitute(void *p) {}
static inline void *FcFontSetMatch(void *config, FcFontSet **sets, int nsets, void *p, int *res) {
    *res = FcResultMatch;
    return p ? strdup((const char *)p) : NULL;
}
static inline FcFontSet *FcFontSort(void *config, void *p, Bool trim, void *cs, int *res) {
    *res = FcResultMatch;
    FcFontSet *s = malloc(sizeof(FcFontSet));
    s->nfont = 0;
    return s;
}

#endif // XFT_COMPAT_H
