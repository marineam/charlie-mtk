#include <assert.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <cairo.h>
#include <cairo-xcb.h>
#include <stdlib.h>
#include <mtk.h>
#include "private.h"

int _screen_num;
xcb_connection_t *_conn;
xcb_screen_t *_screen;
xcb_visualtype_t *_visual;
mtk_list_t *_windows;

void mtk_init()
{
	/* open connection with the server */
	_conn = xcb_connect(NULL, &_screen_num);
	die_on(xcb_connection_has_error(_conn), "Cannot open display\n");

	/* get the first screen and visual*/
	_screen = xcb_aux_get_screen(_conn, _screen_num);
	_visual = xcb_aux_get_visualtype(_conn, _screen_num,
		_screen->root_visual);

	xcb_flush(_conn);

	_windows = mtk_list_new();
}

void mtk_cleanup()
{
	xcb_disconnect(_conn);
}

/* for use in mtk_event, searches for the window the
 * event applies to and runs the given code for it */
#define WINDOW_EVENT(type, member, code) \
	{ type *_e = (type*)e; \
	mtk_list_foreach(_windows, w) { \
		if (w->id == _e->member) { \
			code; \
			xcb_flush(_conn); \
			break; \
		} \
	} \
	assert(w); } /* w should have been found */

int mtk_event()
{
	xcb_generic_event_t *e;
	mtk_window_t *w = NULL;

	e = xcb_poll_for_event(_conn);

	if (xcb_connection_has_error(_conn))
		return -1;

	if (!e)
		return 0; /* nothing to do */

	switch (e->response_type & ~0x80) {
	case XCB_BUTTON_PRESS:
		WINDOW_EVENT(xcb_button_press_event_t, event,
			_mtk_window_click(w, _e->event_x, _e->event_y));
		break;
	case XCB_MOTION_NOTIFY:
		WINDOW_EVENT(xcb_motion_notify_event_t, event,
			_mtk_window_click(w, _e->event_x, _e->event_y));
		break;
	case XCB_EXPOSE:    /* draw or redraw the window */
		WINDOW_EVENT(xcb_expose_event_t, window, _mtk_window_draw(w));
		break;
	case XCB_CONFIGURE_NOTIFY:
		WINDOW_EVENT(xcb_configure_notify_event_t, window,
			_mtk_window_resize(w, _e->width, _e->height));
		break;
	default:
		/* ignore everything else */
		break;
	}

	free(e);
	return 0;
}
