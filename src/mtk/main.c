#include <assert.h>
#include <X11/Xlib.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>
#include <mtk.h>
#include "private.h"

int _screen;
Display *_display;
Visual *_visual;
mtk_list_t *_windows;

void mtk_init()
{
	sigset_t timer;

	sigemptyset(&timer);
	sigaddset(&timer, TIMER_SIG);
	sigprocmask(SIG_BLOCK, &timer, NULL);

	/* fill up class structures */
	_mtk_object_class_init();
	_mtk_widget_class_init();
	_mtk_container_class_init();
	_mtk_window_class_init();
	_mtk_hpack_class_init();
	_mtk_vpack_class_init();
	_mtk_menu_class_init();
	_mtk_text_class_init();
	_mtk_image_class_init();
	_mtk_text_list_class_init();

	/* start of event and timer subsystems */
	_mtk_event_init();
	_mtk_timer_init();

	/* open connection with the server */
	_display = XOpenDisplay(NULL);
	die_on(!_display, "Cannot open display\n");

	/* get the screen and visual*/
	_screen = DefaultScreen(_display);
	_visual = DefaultVisual(_display, _screen);

	_windows = mtk_list_new();
}

void mtk_cleanup()
{
	XCloseDisplay(_display);
	mtk_list_free_obj(_windows);
}

/* for use in mtk_event, searches for the window the
 * event applies to and runs the given code for it */
#define WINDOW_EVENT(type) \
	{ type *_e = (type*)&e; \
	mtk_list_foreach(_windows, w) { \
		if (w->id == _e->window) {
#define WINDOW_EVENT_END \
			break; \
		} \
	} \
	assert(w); } /* w should have been found */

static bool event()
{
	XEvent e;
	mtk_window_t *w = NULL;

	if (!XPending(_display))
		return false; /* nothing to do */

	XNextEvent(_display, &e);

//	if (xcb_connection_has_error(_conn))
//		return false;

	switch (e.type) {
	case ButtonPress:
		WINDOW_EVENT(XButtonEvent)
			call(w,mouse_press, _e->x, _e->y);
		WINDOW_EVENT_END
		break;
	case ButtonRelease:
		WINDOW_EVENT(XButtonEvent)
			call(w,mouse_release, _e->x, _e->y);
		WINDOW_EVENT_END
		break;
	case MotionNotify:
		WINDOW_EVENT(XMotionEvent)
			call(w,mouse_move, _e->x, _e->y);
		WINDOW_EVENT_END
		break;
	case Expose:    /* draw or redraw the window */
		WINDOW_EVENT(XExposeEvent)
			call(w,redraw);
		WINDOW_EVENT_END
		break;
	case ConfigureNotify:
		WINDOW_EVENT(XConfigureEvent)
			call(w,set_size, _e->width, _e->height);
		WINDOW_EVENT_END
		break;
	default:
		/* ignore everything else */
		break;
	}

	return true;
}

void mtk_main()
{
	int xfd, nfds;
	fd_set xfd_set;
	sigset_t signals;

	sigprocmask(SIG_SETMASK, NULL, &signals);
	sigdelset(&signals, TIMER_SIG);

	xfd = ConnectionNumber(_display);
	nfds = xfd+1;
	FD_ZERO(&xfd_set);
	FD_SET(xfd, &xfd_set);

	while (1) {
		bool e = event();
		e |= _mtk_event();
		//xcb_flush(_conn);

		//if (xcb_connection_has_error(_conn))
		//	return;

		/* pause until X sends something or we get a signal */
		if (!e) {
			_mtk_timer_cleanup();
			pselect(nfds, &xfd_set, NULL, NULL, NULL, &signals);
		}
	}
}
