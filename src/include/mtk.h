#ifndef MTK_H
#define MTK_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <xcb/xcb.h>
#include <cairo.h>

#include "config.h"

#include "mtk_util.h"
#include "mtk_object.h"

/* The data in the following structures should only be accessed
 * by the corresponding class implementation in mtk or a child class */
CLASS(mtk_widget, mtk_object)
	int x, y, w, h;
	bool redraw;
	struct mtk_window *window;
	struct mtk_widget *parent;
	cairo_surface_t *surface;
METHODS(mtk_widget, mtk_object)
	void (*init)(void *this, mtk_widget_t* parent);
	void (*draw)(void *this);
	void (*redraw)(void *this);
	void (*mouse_press)(void *this, int x, int y);
	void (*mouse_release)(void *this, int x, int y);
	void (*mouse_move)(void *this, int x, int y);
	void (*set_coord)(void *this, int x, int y);
	void (*set_size)(void *this, int w, int h);
	void (*get_coord)(void *this, int *x, int *y);
	void (*get_size)(void *this, int *w, int *h);
	void (*set_parent)(void *this, mtk_widget_t *parent);
END

CLASS(mtk_container, mtk_widget)
	mtk_list_t *widgets;
	bool ran_init;
	bool event_stacking;
METHODS(mtk_container, mtk_widget)
	void (*add_widget)(void *this, mtk_widget_t *widget);
	void (*reorder_top)(void *this, mtk_widget_t *widget);
END

CLASS(mtk_menu, mtk_container)
	mtk_list_t *menu;
	mtk_widget_t *top, *old;
	int slide;
	int slide_max;
	int slide_item;
	char slide_dir;
	bool slide_active;
METHODS(mtk_menu, mtk_container)
	void (*add_item)(void *this, mtk_widget_t *widget, char *text);
END

CLASS(mtk_window, mtk_container)
	xcb_window_t id;
	//cairo_surface_t *surface;
METHODS(mtk_window, mtk_container, int w, int h)
END

CLASS(mtk_hpack, mtk_container)
	mtk_list_t *order;
METHODS(mtk_hpack, mtk_container)
	void (*pack_left)(void *this, mtk_widget_t *widget, int w);
	void (*pack_right)(void *this, mtk_widget_t *widget, int w);
END

CLASS(mtk_vpack, mtk_container)
	mtk_list_t *order;
METHODS(mtk_vpack, mtk_container)
	void (*pack_top)(void *this, mtk_widget_t *widget, int h);
	void (*pack_bottom)(void *this, mtk_widget_t *widget, int h);
END

CLASS(mtk_text, mtk_widget)
	char *text;
	int scroll;
	bool scroll_stop;
METHODS(mtk_text, mtk_widget, char *text)
	void (*set_text)(void *this, char *text);
END

CLASS(mtk_image, mtk_widget)
	char *path;
	cairo_surface_t *image;
METHODS(mtk_image, mtk_widget, char *path)
	void (*set_image)(void *this, char *path);
END

CLASS(mtk_text_list, mtk_widget)
	mtk_list_t* list;
	int scroll_dir;
	int scroll_top;
	bool scroll_active;
	bool scroll_hold;
	cairo_surface_t *cache;
	int cache_h;
METHODS(mtk_text_list, mtk_widget, mtk_list_t *list)
	void (*set_list)(void *this, mtk_list_t *list);
	char* (*_item_text)(void *this, void *item);
	void (*_item_draw)(void *this, cairo_t *cr, void *item, int y);
	void (*_item_click)(void *this, void *item);
	void (*_item_free)(void *this, void *item);
END

/* main.c */
void mtk_init();
void mtk_cleanup();
void mtk_main();

/* event.c */
void mtk_event_add(void(*callback)(void *data), void *data);

/* timer.c */
void mtk_timer_add(double interval, bool(*callback)(void *data), void *data);
double mtk_time();
void mtk_sleep(double sec);

#endif
