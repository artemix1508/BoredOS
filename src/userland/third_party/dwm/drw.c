/* See LICENSE file for copyright and license details.
 * drw.c — BoredOS port: core X fonts only (XFontStruct / XDrawString).
 * All Xft / FreeType / fontconfig code has been removed.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

#include "drw.h"
#include "util.h"

/* -------------------------------------------------------------------------
 * UTF-8 helpers — retained from original drw.c so text widths still work.
 * ------------------------------------------------------------------------- */
#define UTF_INVALID 0xFFFD
#define UTF_SIZ     4

static const unsigned char utfbyte[UTF_SIZ + 1] = {0x80,    0, 0xC0, 0xE0, 0xF0};
static const unsigned char utfmask[UTF_SIZ + 1] = {0xC0, 0x80, 0xE0, 0xF0, 0xF8};
static const long utfmin[UTF_SIZ + 1] = {       0,    0,  0x80,  0x800,  0x10000};
static const long utfmax[UTF_SIZ + 1] = {0x10FFFF, 0x7F, 0x7FF, 0xFFFF, 0x10FFFF};

static long
utf8decodebyte(const char c, size_t *i)
{
	for (*i = 0; *i < (UTF_SIZ + 1); ++(*i))
		if (((unsigned char)c & utfmask[*i]) == utfbyte[*i])
			return (unsigned char)c & ~utfmask[*i];
	return 0;
}

static size_t
utf8decode(const char *c, long *u, size_t clen)
{
	size_t i, j, len, type;
	long udecoded;

	*u = UTF_INVALID;
	if (!clen)
		return 0;
	udecoded = utf8decodebyte(c[0], &len);
	if (!BETWEEN(len, 1, UTF_SIZ))
		return 1;
	for (i = 1, j = 1; i < clen && j < len; ++i, ++j) {
		udecoded = (udecoded << 6) | utf8decodebyte(c[i], &type);
		if (type)
			return j;
	}
	if (j < len)
		return 0;
	*u = udecoded;
	return len;
}

/* -------------------------------------------------------------------------
 * Drawable
 * ------------------------------------------------------------------------- */
Drw *
drw_create(Display *dpy, int screen, Window root, unsigned int w, unsigned int h)
{
	Drw *drw = ecalloc(1, sizeof(Drw));

	drw->dpy      = dpy;
	drw->screen   = screen;
	drw->root     = root;
	drw->w        = w;
	drw->h        = h;
	drw->drawable = XCreatePixmap(dpy, root, w, h, DefaultDepth(dpy, screen));
	drw->gc       = XCreateGC(dpy, root, 0, NULL);
	XSetLineAttributes(dpy, drw->gc, 1, LineSolid, CapButt, JoinMiter);

	return drw;
}

void
drw_resize(Drw *drw, unsigned int w, unsigned int h)
{
	if (!drw)
		return;

	drw->w = w;
	drw->h = h;
	if (drw->drawable)
		XFreePixmap(drw->dpy, drw->drawable);
	drw->drawable = XCreatePixmap(drw->dpy, drw->root, w, h,
	                              DefaultDepth(drw->dpy, drw->screen));
}

void
drw_free(Drw *drw)
{
	XFreePixmap(drw->dpy, drw->drawable);
	XFreeGC(drw->dpy, drw->gc);
	drw_fontset_free(drw->fonts);
	free(drw);
}

/* -------------------------------------------------------------------------
 * Font (core X11 — XFontStruct via XLoadQueryFont)
 * ------------------------------------------------------------------------- */
static Fnt *
xfont_create(Drw *drw, const char *fontname)
{
	Fnt         *font;
	XFontStruct *xfont;

	if (!fontname) {
		die("drw: no font specified");
	}

	xfont = XLoadQueryFont(drw->dpy, fontname);
	if (!xfont) {
		/* Try the universal fallback */
		fprintf(stderr, "drw: cannot load font '%s', trying 'fixed'\n", fontname);
		xfont = XLoadQueryFont(drw->dpy, "fixed");
	}
	if (!xfont) {
		fprintf(stderr, "drw: cannot load fallback font 'fixed'\n");
		return NULL;
	}

	font        = ecalloc(1, sizeof(Fnt));
	font->xfont = xfont;
	font->h     = xfont->ascent + xfont->descent;
	font->dpy   = drw->dpy;
	font->next  = NULL;

	return font;
}

static void
xfont_free(Fnt *font)
{
	if (!font)
		return;
	if (font->xfont)
		XFreeFont(font->dpy, font->xfont);
	free(font);
}

Fnt *
drw_fontset_create(Drw *drw, const char *fonts[], size_t fontcount)
{
	Fnt *cur, *ret = NULL;
	size_t i;

	if (!drw || !fonts)
		return NULL;

	for (i = 1; i <= fontcount; i++) {
		if ((cur = xfont_create(drw, fonts[fontcount - i]))) {
			cur->next = ret;
			ret = cur;
		}
	}
	return (drw->fonts = ret);
}

void
drw_fontset_free(Fnt *font)
{
	if (font) {
		drw_fontset_free(font->next);
		xfont_free(font);
	}
}

/* -------------------------------------------------------------------------
 * Colors (XAllocNamedColor — no Xft)
 * ------------------------------------------------------------------------- */
void
drw_clr_create(Drw *drw, Clr *dest, const char *clrname)
{
	XColor exact;

	if (!drw || !dest || !clrname)
		return;

	if (!XAllocNamedColor(drw->dpy,
	                      DefaultColormap(drw->dpy, drw->screen),
	                      clrname, dest, &exact))
		die("drw: cannot allocate color '%s'", clrname);
}

Clr *
drw_scm_create(Drw *drw, const char *clrnames[], size_t clrcount)
{
	size_t i;
	Clr   *ret;

	if (!drw || !clrnames || clrcount < 2 ||
	    !(ret = ecalloc(clrcount, sizeof(XColor))))
		return NULL;

	for (i = 0; i < clrcount; i++)
		drw_clr_create(drw, &ret[i], clrnames[i]);
	return ret;
}

/* -------------------------------------------------------------------------
 * Drawing context
 * ------------------------------------------------------------------------- */
void
drw_setfontset(Drw *drw, Fnt *set)
{
	if (drw)
		drw->fonts = set;
}

void
drw_setscheme(Drw *drw, Clr *scm)
{
	if (drw)
		drw->scheme = scm;
}

/* -------------------------------------------------------------------------
 * Drawing primitives
 * ------------------------------------------------------------------------- */
void
drw_rect(Drw *drw, int x, int y, unsigned int w, unsigned int h,
         int filled, int invert)
{
	if (!drw || !drw->scheme)
		return;
	XSetForeground(drw->dpy, drw->gc,
	               invert ? drw->scheme[ColBg].pixel
	                      : drw->scheme[ColFg].pixel);
	if (filled)
		XFillRectangle(drw->dpy, drw->drawable, drw->gc, x, y, w, h);
	else
		XDrawRectangle(drw->dpy, drw->drawable, drw->gc, x, y, w - 1, h - 1);
}

int
drw_text(Drw *drw, int x, int y, unsigned int w, unsigned int h,
         unsigned int lpad, const char *text, int invert)
{
	int        render = x || y || w || h;
	int        ty;
	unsigned int tw;
	Fnt       *usedfont;
	char       buf[1024];
	size_t     len;

	if (!drw || (render && (!drw->scheme || !w)) || !text || !drw->fonts)
		return 0;

	if (!render) {
		/* Width-measurement mode: return total pixel width */
		usedfont = drw->fonts;
		if (!usedfont || !usedfont->xfont)
			return 0;
		return XTextWidth(usedfont->xfont, text, (int)strlen(text));
	}

	/* Clip text to buffer */
	len = strlen(text);
	if (len >= sizeof(buf))
		len = sizeof(buf) - 1;
	memcpy(buf, text, len);
	buf[len] = '\0';

	usedfont = drw->fonts;
	if (!usedfont || !usedfont->xfont)
		return x + (int)w;

	/* Background fill */
	XSetForeground(drw->dpy, drw->gc,
	               drw->scheme[invert ? ColFg : ColBg].pixel);
	XFillRectangle(drw->dpy, drw->drawable, drw->gc, x, y, w, h);

	/* Text width — trim if needed */
	tw = (unsigned int)XTextWidth(usedfont->xfont, buf, (int)len);
	while (tw > w - lpad && len > 0) {
		len--;
		tw = (unsigned int)XTextWidth(usedfont->xfont, buf, (int)len);
	}

	if (len > 0) {
		ty = y + (int)(h - usedfont->h) / 2 + usedfont->xfont->ascent;
		XSetFont(drw->dpy, drw->gc, usedfont->xfont->fid);
		XSetForeground(drw->dpy, drw->gc,
		               drw->scheme[invert ? ColBg : ColFg].pixel);
		XDrawString(drw->dpy, drw->drawable, drw->gc,
		            x + (int)lpad, ty, buf, (int)len);
	}

	return x + (int)w;
}

void
drw_map(Drw *drw, Window win, int x, int y, unsigned int w, unsigned int h)
{
	if (!drw)
		return;

	XCopyArea(drw->dpy, drw->drawable, win, drw->gc, x, y, w, h, x, y);
	XSync(drw->dpy, False);
}

/* -------------------------------------------------------------------------
 * Font metric helpers
 * ------------------------------------------------------------------------- */
unsigned int
drw_fontset_getwidth(Drw *drw, const char *text)
{
	if (!drw || !drw->fonts || !text)
		return 0;
	if (!drw->fonts->xfont)
		return 0;
	return (unsigned int)XTextWidth(drw->fonts->xfont, text, (int)strlen(text));
}

unsigned int
drw_fontset_getwidth_clamp(Drw *drw, const char *text, unsigned int n)
{
	unsigned int tmp = drw_fontset_getwidth(drw, text);
	return MIN(n, tmp);
}

void
drw_font_getexts(Fnt *font, const char *text, unsigned int len,
                 unsigned int *w, unsigned int *h)
{
	if (!font || !text)
		return;
	if (w)
		*w = (unsigned int)XTextWidth(font->xfont, text, (int)len);
	if (h)
		*h = font->h;
}

/* -------------------------------------------------------------------------
 * Cursor
 * ------------------------------------------------------------------------- */
Cur *
drw_cur_create(Drw *drw, int shape)
{
	Cur *cur;

	if (!drw || !(cur = ecalloc(1, sizeof(Cur))))
		return NULL;

	cur->cursor = XCreateFontCursor(drw->dpy, shape);
	return cur;
}

void
drw_cur_free(Drw *drw, Cur *cursor)
{
	if (!cursor)
		return;

	XFreeCursor(drw->dpy, cursor->cursor);
	free(cursor);
}
