#include <assert.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <cairo.h>
#include <cairo-xcb.h>
#include <mtk.h>

#include "private.h"

/* for finding widgets at a location */
#define FOREACH_AT_XY(l,w,x,y) \
	mtk_list_foreach(l, w) \
		if (x >= w->x && y >= w->y && \
		    x <= w->x + w->w && y <= w->y + w->h)

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

	window->surface = cairo_xcb_surface_create(_conn,
		window->id, _visual, w, h);
	xcb_flush(_conn);

	mtk_list_append(_windows, window);

	return window;
}

static void draw_widget(mtk_widget_t* widget) {
	cairo_t *cr = cairo_create(widget->window->surface);

	cairo_rectangle(cr, widget->x, widget->y, widget->w, widget->h);
	cairo_clip(cr);
	cairo_set_source_surface(cr, widget->surface, widget->x, widget->y);
	cairo_paint(cr);

	cairo_destroy(cr);
}

void mtk_window_add(mtk_window_t* window, mtk_widget_t* widget)
{
	mtk_list_append(window->widgets, widget);
	widget->window = window;
	widget->surface = cairo_surface_create_similar(window->surface,
			CAIRO_CONTENT_COLOR_ALPHA,
			window->width, window->height);

	if (widget->update)
		widget->update(widget);
	assert(widget->draw);
	widget->draw(widget);
	draw_widget(widget);
}

void _mtk_window_draw(mtk_window_t *window)
{
	mtk_widget_t *w;
	/*cairo_t *cr;

	cr = cairo_create(window->surface);
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_rectangle(cr, 0, 0, window->width, window->height);
	cairo_fill(cr);
	cairo_destroy(cr);*/

	mtk_list_foreach(window->widgets, w) {
		assert(w->draw);
		w->draw(w);
		draw_widget(w);
	}
}

void _mtk_window_mouse_press(mtk_window_t *window, int x, int y)
{
	mtk_widget_t *w;

	FOREACH_AT_XY(window->widgets, w, x, y)
		if (w->mouse_press)
			w->mouse_press(w, x-w->x, y-w->y);
}

void _mtk_window_mouse_release(mtk_window_t *window, int x, int y)
{
	mtk_widget_t *w;

	FOREACH_AT_XY(window->widgets, w, x, y)
		if (w->mouse_release)
			w->mouse_release(w, x-w->x, y-w->y);
}

void _mtk_window_mouse_move(mtk_window_t *window, int x, int y)
{
	mtk_widget_t *w;

	FOREACH_AT_XY(window->widgets, w, x, y)
		if (w->mouse_move)
			w->mouse_move(w, x-w->x, y-w->y);
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
