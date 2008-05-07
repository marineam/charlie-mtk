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
	/* fill up class structures */
	_mtk_object_class_init();
	_mtk_widget_class_init();
	_mtk_container_class_init();
	_mtk_window_class_init();
	_mtk_text_class_init();
	_mtk_mpdlist_class_init();

	/* fire off timer thread */
	_mtk_timer_init();

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

void _mtk_flush()
{
	xcb_flush(_conn);
}

/* for use in mtk_event, searches for the window the
 * event applies to and runs the given code for it */
#define WINDOW_EVENT(type, member) \
	{ type *_e = (type*)e; \
	mtk_list_foreach(_windows, w) { \
		if (w->id == _e->member) {
#define WINDOW_EVENT_END \
			break; \
		} \
	} \
	assert(w); } /* w should have been found */

static int event()
{
	xcb_generic_event_t *e;
	mtk_window_t *w = NULL;

	e = xcb_wait_for_event(_conn);

	if (xcb_connection_has_error(_conn))
		return -1;

	if (!e)
		return 0; /* nothing to do */

	switch (e->response_type & ~0x80) {
	case XCB_BUTTON_PRESS:
		WINDOW_EVENT(xcb_button_press_event_t, event)
			call(w,mtk_widget,mouse_press,_e->event_x,_e->event_y);
			/* temporary hack to force a redraw */
			call(w,mtk_widget,draw);
		WINDOW_EVENT_END
		break;
	case XCB_BUTTON_RELEASE:
		WINDOW_EVENT(xcb_button_release_event_t, event)
			call(w,mtk_widget,mouse_release,_e->event_x,_e->event_y);
			/* temporary hack to force a redraw */
			call(w,mtk_widget,draw);
		WINDOW_EVENT_END
		break;
	case XCB_MOTION_NOTIFY:
		WINDOW_EVENT(xcb_motion_notify_event_t, event)
			call(w,mtk_widget,mouse_move,_e->event_x,_e->event_y);
			/* temporary hack to force a redraw */
			call(w,mtk_widget,draw);
		WINDOW_EVENT_END
		break;
	case XCB_EXPOSE:    /* draw or redraw the window */
		WINDOW_EVENT(xcb_expose_event_t, window)
			call(w,mtk_widget,draw);
		WINDOW_EVENT_END
		break;
	case XCB_CONFIGURE_NOTIFY:
		WINDOW_EVENT(xcb_configure_notify_event_t, window)
			call(w,mtk_window,resize, _e->width, _e->height);
		WINDOW_EVENT_END
		break;
	default:
		/* ignore everything else */
		break;
	}

	free(e);
	return 1;
}

void mtk_main()
{
	int xfd, nfds;
	fd_set xfd_set;
	mtk_window_t *w;
	sigset_t sigset;

	sigemptyset(&sigset);
	sigaddset(&sigset, TIMER_SIG);
	sigprocmask(SIG_BLOCK, &sigset, NULL);

	xfd = xcb_get_file_descriptor(_conn);
	nfds = xfd;
	FD_ZERO(&xfd_set);
	FD_SET(xfd, &rfds);

	while (1) {
		pselect(nfds, &xfd_set, &xfd_set, NULL, NULL
		if ((xev = event()) < 0)
			break;

		if ((tev = _mtk_timer_event())) {
			/* FIXME: force a full redraw on timer events.
			 * this is silly but I don't have a better way yet */
		mtk_list_foreach(_windows, w) {
			call(w,mtk_widget,draw);
		}

		if (!xev && !tev) {
			/* no events that time, so lets just idle a bit */
			nanosleep(&pause,NULL);
		}
	}
}
