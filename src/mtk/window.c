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

	call(window,mtk_widget,set_geometry, 0, 0, w, h);

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

	call(window,mtk_widget,init, NULL);

	return window;
}

static void resize(mtk_window_t *window, int w, int h)
{
	if (w == mtk_widget(window)->w && h == mtk_widget(window)->h)
		return; /* no change in size */
	cairo_xcb_surface_set_size(mtk_widget(window)->surface, w, h);
	mtk_widget(window)->w = w;
	mtk_widget(window)->h = h;
	/* TODO: reisize and signal child widgets somehow? */
	call(window,mtk_widget,draw);
}

METHOD_TABLE_INIT(mtk_window, mtk_container)
	METHOD(resize);
METHOD_TABLE_END
