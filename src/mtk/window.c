#include <assert.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <cairo.h>
#include <cairo-xcb.h>
#include <mtk.h>

#include "private.h"

mtk_window_t* mtk_window_new(size_t size, int w, int h)
{
	mtk_window_t *window = mtk_window(mtk_container_new(size));
	xcb_params_cw_t values;
	uint32_t mask;

	SET_CLASS(window, mtk_window);

	mtk_widget(window)->w = w;
	mtk_widget(window)->h = h;

	/* create window */
	window->id = xcb_generate_id(_conn);

	mask = XCB_CW_EVENT_MASK;
	values.event_mask =
		XCB_EVENT_MASK_EXPOSURE |
		XCB_EVENT_MASK_BUTTON_PRESS |
		XCB_EVENT_MASK_BUTTON_RELEASE |
		XCB_EVENT_MASK_BUTTON_MOTION |
		XCB_EVENT_MASK_STRUCTURE_NOTIFY;
	xcb_aux_create_window(_conn, _screen->root_depth,
		window->id, _screen->root,
		0, 0, w, h, 0, /* x,y,w,h,border */
		XCB_WINDOW_CLASS_INPUT_OUTPUT,
		_screen->root_visual, mask, &values);

	/* show the window */
	xcb_map_window(_conn, window->id);

	mtk_widget(window)->surface = cairo_xcb_surface_create(_conn,
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
	cairo_xcb_surface_set_size(this->surface, w, h);

	super(this,mtk_window,set_size, w, h);
}

METHOD_TABLE_INIT(mtk_window, mtk_container)
	METHOD(redraw);
	METHOD(set_size);
METHOD_TABLE_END
