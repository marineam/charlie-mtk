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

int mtk_event()
{
	xcb_generic_event_t *e;
	xcb_expose_event_t *ee;
	xcb_configure_notify_event_t *ce;
	xcb_button_press_event_t *be;
	xcb_motion_notify_event_t *me;
	mtk_window_t *w = NULL;
	mtk_list_node_t *n;

	e = xcb_poll_for_event(_conn);

	if (xcb_connection_has_error(_conn))
		return -1;

	if (!e)
		return 0; /* nothing to do */

	switch (e->response_type & ~0x80) {
		case XCB_BUTTON_PRESS:
			be = (xcb_button_press_event_t*)e;

			for (n = _windows->first; n; n = n->next) {
				w = n->data;
				if (w->id == be->event) {
					_mtk_window_click(w,
						be->event_x, be->event_y);
					xcb_flush(_conn);
					break;
				}
			}
			assert(w); /* w should have been found */

			break;
		case XCB_MOTION_NOTIFY:
			me = (xcb_motion_notify_event_t*)e;

			for (n = _windows->first; n; n = n->next) {
				w = n->data;
				if (w->id == me->event) {
					_mtk_window_click(w,
						me->event_x, me->event_y);
					xcb_flush(_conn);
					break;
				}
			}
			assert(w); /* w should have been found */

			break;
		case XCB_EXPOSE:    /* draw or redraw the window */
			ee = (xcb_expose_event_t*)e;

			for (n = _windows->first; n; n = n->next) {
				w = n->data;
				if (w->id == ee->window) {
					_mtk_window_draw(w);
					xcb_flush(_conn);
					break;
				}
			}
			assert(w); /* w should have been found */

			break;
		case XCB_CONFIGURE_NOTIFY:
			ce = (xcb_configure_notify_event_t*)e;

			for (n = _windows->first; n; n = n->next) {
				w = n->data;
				if (w->id == ce->window) {
					_mtk_window_resize(w,
						ce->width, ce->height);
					xcb_flush(_conn);
					break;
				}
			}
			assert(w); /* w should have been found */

			break;
		default:
			/* ignore everything else */
			break;
	}

	free(e);
	return 0;
}
