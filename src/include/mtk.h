#ifndef MTK_H
#define MTK_H

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
METHODS(mtk_widget, mtk_object, int x, int y, int w, int h)
	void (*init)(mtk_widget_t *this, mtk_widget_t* parent);
	void (*draw)(mtk_widget_t *this); /* children must implement this */
	void (*update)(mtk_widget_t *this);
	void (*mouse_press)(mtk_widget_t *this, int x, int y);
	void (*mouse_release)(mtk_widget_t *this, int x, int y);
	void (*mouse_move)(mtk_widget_t *this, int x, int y);
	void (*set_geometry)(mtk_widget_t *this, int x, int y, int w, int h);
	void (*set_parent)(mtk_widget_t *this, mtk_widget_t *parent);
END

CLASS(mtk_container, mtk_widget)
	mtk_list_t *widgets;
	int ran_init;
METHODS(mtk_container, mtk_widget, int x, int y, int w, int h)
	void (*add_widget)(mtk_container_t *this, mtk_widget_t *widget);
END

CLASS(mtk_window, mtk_container)
	xcb_window_t id;
	//cairo_surface_t *surface;
METHODS(mtk_window, mtk_container, int w, int h)
	void (*resize)(mtk_window_t* this, int w, int h);
END

CLASS(mtk_text, mtk_widget)
	char *text;
	int scroll;
METHODS(mtk_text, mtk_widget, int x, int y, int w, int h, char *text)
	void (*set_text)(mtk_text_t *this, char *text);
END

CLASS(mtk_image, mtk_widget)
	char *path;
	cairo_surface_t *image;
METHODS(mtk_image, mtk_widget, int x, int y, int w, int h, char *path)
	void (*set_image)(mtk_image_t *this, char *path);
END

CLASS(mtk_mpdlist, mtk_widget)
	mtk_list_t* list;
	int timed_scroll;
	int timed_active;
	int slide_scroll;
	int slide_start;
	int slide_offset;
	int slide_scroll_moved;
	int scroll_top;
	cairo_surface_t *scroll_surface;
	void (*updatelist)(mtk_list_t *list, void *data);
	int (*clicked)(void **data, mtk_list_t *list, int pos);
	void *data;
METHODS(mtk_mpdlist, mtk_widget, int x, int y, int w, int h,
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
void mtk_timer_add(double interval, int(*callback)(void *data), void *data);
/* Hack to make libmpdclient not shit itself by blocking SIGALRM */
void mtk_timer_block();
void mtk_timer_unblock();

#endif
