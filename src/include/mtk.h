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
	struct mtk_window *window;
	struct mtk_widget *parent;
	cairo_surface_t *surface;
METHODS(mtk_widget, mtk_object)
	void (*init)(mtk_widget_t *this, mtk_widget_t* parent);
	void (*draw)(mtk_widget_t *this); /* children must implement this */
	void (*update)(mtk_widget_t *this);
	void (*mouse_press)(mtk_widget_t *this, int x, int y);
	void (*mouse_release)(mtk_widget_t *this, int x, int y);
	void (*mouse_move)(mtk_widget_t *this, int x, int y);
	void (*set_coord)(mtk_widget_t *this, int x, int y);
	void (*set_size)(mtk_widget_t *this, int w, int h);
	void (*get_coord)(mtk_widget_t *this, int *x, int *y);
	void (*get_size)(mtk_widget_t *this, int *w, int *h);
	void (*set_parent)(mtk_widget_t *this, mtk_widget_t *parent);
END

CLASS(mtk_container, mtk_widget)
	mtk_list_t *widgets;
	bool ran_init;
	bool event_stacking;
METHODS(mtk_container, mtk_widget)
	void (*add_widget)(mtk_container_t *this, mtk_widget_t *widget);
	void (*reorder_top)(mtk_container_t *this, mtk_widget_t *widget);
END

CLASS(mtk_viewer, mtk_container)
	mtk_widget_t *slider;
	int slide;
METHODS(mtk_viewer, mtk_container)
END

CLASS(mtk_window, mtk_container)
	xcb_window_t id;
	//cairo_surface_t *surface;
METHODS(mtk_window, mtk_container, int w, int h)
END

CLASS(mtk_hpack, mtk_container)
	mtk_list_t *order;
METHODS(mtk_hpack, mtk_container)
	void (*pack_left)(mtk_hpack_t *this, mtk_widget_t *widget, int w);
	void (*pack_right)(mtk_hpack_t *this, mtk_widget_t *widget, int w);
END

CLASS(mtk_vpack, mtk_container)
	mtk_list_t *order;
METHODS(mtk_vpack, mtk_container)
	void (*pack_top)(mtk_vpack_t *this, mtk_widget_t *widget, int h);
	void (*pack_bottom)(mtk_vpack_t *this, mtk_widget_t *widget, int h);
END

CLASS(mtk_text, mtk_widget)
	char *text;
	int scroll;
	bool scroll_stop;
METHODS(mtk_text, mtk_widget, char *text)
	void (*set_text)(mtk_text_t *this, char *text);
END

CLASS(mtk_image, mtk_widget)
	char *path;
	cairo_surface_t *image;
METHODS(mtk_image, mtk_widget, char *path)
	void (*set_image)(mtk_image_t *this, char *path);
END

CLASS(mtk_mpdlist, mtk_widget)
	mtk_list_t* list;
	int timed_scroll;
	bool timed_active;
	bool slide_scroll;
	bool slide_scroll_moved;
	int slide_start;
	int slide_offset;
	int scroll_top;
	cairo_surface_t *scroll_surface;
	void (*updatelist)(mtk_list_t *list, void *data);
	int (*clicked)(void **data, mtk_list_t *list, int pos);
	void *data;
METHODS(mtk_mpdlist, mtk_widget,
	void (*updatelist)(mtk_list_t *list, void *data),
	int (*clicked)(void **data, mtk_list_t *list, int pos),
	void *data)
END

/* main.c */
void mtk_init();
void mtk_cleanup();
void mtk_main();

/* event.c */
void mtk_event_add(void(*callback)(void *data), void *data);
void mtk_event_process();

/* timer.c */
void mtk_timer_add(double interval, bool(*callback)(void *data), void *data);
/* Hack to make libmpdclient not shit itself by blocking SIGALRM */
void mtk_timer_block();
void mtk_timer_unblock();

#endif
