#include <assert.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <cairo.h>
#include <cairo-xcb.h>
#include <mtk.h>

#include "private.h"

mtk_window_t* mtk_window_new(int w, int h)
{
	mtk_window_t *window = xmalloc0(sizeof(mtk_window_t));
	xcb_params_cw_t values;
	uint32_t mask;

	/* create window */
	window->id = xcb_generate_id(_conn);
	window->width = w;
	window->height = h;
	window->widgets = mtk_list_new();
	mask = XCB_CW_EVENT_MASK;
	values.event_mask =
		XCB_EVENT_MASK_EXPOSURE |
		XCB_EVENT_MASK_BUTTON_PRESS |
		XCB_EVENT_MASK_BUTTON_MOTION |
		XCB_EVENT_MASK_STRUCTURE_NOTIFY;
	xcb_aux_create_window(_conn, _screen->root_depth,
		window->id, _screen->root,
		0, 0, w, h, 0, /* x,y,w,h,border */
		XCB_WINDOW_CLASS_INPUT_OUTPUT,
		_screen->root_visual, mask, &values);

	/* show the window */
	xcb_map_window(_conn, window->id);

	window->surface = cairo_xcb_surface_create(_conn,
		window->id, _visual, w, h);
	xcb_flush(_conn);

	mtk_list_append(_windows, window);

	return window;
}

void mtk_window_add(mtk_window_t* window, mtk_widget_t* widget)
{
	mtk_list_append(window->widgets, widget);
	widget->window = window;
	widget->draw(widget);
}

void _mtk_window_draw(mtk_window_t *window)
{
	mtk_widget_t *w;
	cairo_t *cr;

	cr = cairo_create(window->surface);
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_rectangle(cr, 0, 0, window->width, window->height);
	cairo_fill(cr);
	cairo_destroy(cr);

	mtk_list_foreach(window->widgets, w) {
		assert(w->draw);
		w->draw(w);
	}
}

void _mtk_window_click(mtk_window_t *window, int x, int y)
{
	mtk_widget_t *w;

	mtk_list_foreach(window->widgets, w) {
		if (w->click && x >= w->x && y >= w->y &&
				x <= w->x + w->w && y <= w->y + w->h)
			w->click(w, x-w->x, y-w->y);
	}
}

void _mtk_window_resize(mtk_window_t *window, int w, int h)
{
	if (w == window->width && h == window->height)
		return; /* no change in size */
	cairo_xcb_surface_set_size(window->surface, w, h);
	window->width = w;
	window->height = h;
	_mtk_window_draw(window);
}
