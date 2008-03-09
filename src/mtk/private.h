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
	void (*draw)(mtk_widget_t *widget);
	void (*click)(mtk_widget_t *widget, int x, int y);
};

extern int _screen_num;
extern xcb_connection_t *_conn;
extern xcb_screen_t *_screen;
extern xcb_visualtype_t *_visual;
extern mtk_list_t *_windows;

void _mtk_window_draw(mtk_window_t *window);
void _mtk_window_click(mtk_window_t *window, int x, int y);
void _mtk_window_resize(mtk_window_t *window, int w, int h);

#endif