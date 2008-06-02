#include <assert.h>
#include <X11/Xlib.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include <mtk.h>

#include "private.h"

mtk_window_t* mtk_window_new(size_t size, int w, int h)
{
	mtk_window_t *window = mtk_window(mtk_container_new(size));

	SET_CLASS(window, mtk_window);

	mtk_widget(window)->w = w;
	mtk_widget(window)->h = h;

	/* create window */
	window->id = XCreateSimpleWindow(_display,
		RootWindow(_display,_screen),
		0, 0, w, h, 0, /* x,y,w,h,border */
		WhitePixel(_display, _screen),
		WhitePixel(_display, _screen));

	XSelectInput(_display, window->id,
		ExposureMask |
		ButtonPressMask |
		ButtonReleaseMask |
		ButtonMotionMask |
		StructureNotifyMask);

	/* show the window */
	XMapWindow(_display, window->id);

	mtk_widget(window)->surface = cairo_xlib_surface_create(_display,
		window->id, _visual, w, h);

	mtk_list_append(_windows, window);

	call(window,init, NULL);

	return window;
}

static void start_draw(void *data)
{
	mtk_window_t *w = data;

	call(w,draw);
}

static void redraw(void *vthis)
{
	mtk_widget_t *this = vthis;

	if (this->redraw)
		return;

	this->redraw = true;
	mtk_event_add(start_draw, this);
}

static void set_size(void *vthis, int w, int h)
{
	mtk_widget_t *this = vthis;

	if (this->w == w && this->h == h)
		return;

	assert(this->surface);
	cairo_xlib_surface_set_size(this->surface, w, h);

	super(this,mtk_window,set_size, w, h);
}

METHOD_TABLE_INIT(mtk_window, mtk_container)
	METHOD(redraw);
	METHOD(set_size);
METHOD_TABLE_END
