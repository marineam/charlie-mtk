#ifndef _PRIVATE_H
#define _PRIVATE_H

#include <xcb/xcb.h>
#include <cairo.h>
#include <mtk.h>

struct mtk_window {
	xcb_window_t id;
	cairo_surface_t *surface;
	int width;
	int height;
	mtk_list_t *widgets;
};

struct mtk_widget {
	int x, y, w, h;
	mtk_window_t *window;
	cairo_surface_t *surface;
	void (*draw)(mtk_widget_t *widget); /* required */
	void (*update)(mtk_widget_t *widget);
	void (*mouse_press)(mtk_widget_t *widget, int x, int y);
	void (*mouse_release)(mtk_widget_t *widget, int x, int y);
	void (*mouse_move)(mtk_widget_t *widget, int x, int y);
};

extern int _screen_num;
extern xcb_connection_t *_conn;
extern xcb_screen_t *_screen;
extern xcb_visualtype_t *_visual;
extern mtk_list_t *_windows;

void _mtk_flush();

void _mtk_window_draw(mtk_window_t *window);
void _mtk_window_mouse_press(mtk_window_t *window, int x, int y);
void _mtk_window_mouse_release(mtk_window_t *window, int x, int y);
void _mtk_window_mouse_move(mtk_window_t *window, int x, int y);
void _mtk_window_resize(mtk_window_t *window, int w, int h);

void _mtk_timer_init();
int _mtk_timer_event();

#endif
