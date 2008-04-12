#ifndef MTK_H
#define MTK_H

#include <stdlib.h>
#include <stdio.h>
#include <xcb/xcb.h>
#include <cairo.h>
#include <libmpdclient.h>

#include "config.h"

typedef struct mtk_list_node {
	struct mtk_list_node *next;
	struct mtk_list_node *prev;
	void* data;
} mtk_list_node_t;

typedef struct mtk_list {
	struct mtk_list_node *first;
	struct mtk_list_node *last;
	struct mtk_list_node *current_node;
	int current_index;
	int count;
} mtk_list_t;

typedef struct mtk_widget mtk_widget_t;
typedef struct mtk_container mtk_container_t;
typedef struct mtk_window mtk_window_t;
typedef struct mtk_text mtk_text_t;

/* TODO: add a compile time check to make sure these casts are valid */
#define MTK_WIDGET(w) ((mtk_widget_t*)w)
#define MTK_CONTAINER(c) ((mtk_container_t*)c)
#define MTK_WINDOW(w) ((mtk_window_t*)w)
#define MTK_TEXT(t) ((mtk_text_t*)t)

/* The data in the following structures should only be accessed
 * by the corresponding class implementation in mtk or a child class */
struct mtk_widget {
	int x, y, w, h;
	mtk_window_t *window;
	cairo_surface_t *surface;
	void (*init)(mtk_widget_t* widget, mtk_widget_t* parent);
	void (*draw)(mtk_widget_t *widget); /* required */
	void (*update)(mtk_widget_t *widget);
	void (*mouse_press)(mtk_widget_t *widget, int x, int y);
	void (*mouse_release)(mtk_widget_t *widget, int x, int y);
	void (*mouse_move)(mtk_widget_t *widget, int x, int y);
};

struct mtk_container {
	mtk_widget_t widget;
	mtk_list_t *widgets;
	int ran_init;
	void (*parent_init)(mtk_widget_t* widget, mtk_widget_t* parent);
};

struct mtk_window {
	mtk_container_t container;
	xcb_window_t id;
	//cairo_surface_t *surface;
};

struct mtk_text {
	mtk_widget_t widget;
	char *text;
	double size;
};

/* main.c */
void mtk_init();
void mtk_cleanup();
int mtk_event(int block);
void mtk_main();

/* widget.c */
void _mtk_widget_new(mtk_widget_t *widget, int x, int y, int w, int h);

/* clickarea.c */
mtk_widget_t* mtk_clickarea_new(int x, int y, int w, int h);

/* mpdlist.c */
mtk_widget_t* mtk_mpdlist_new(int x, int y, int w, int h,
	void (*updatelist)(mtk_list_t *list, void *data),
	int (*clicked)(void **data, mtk_list_t *list, int pos),
	void *data);

/* container.c */
void _mtk_container_new(mtk_container_t *c, int x, int y, int w, int h);
mtk_container_t* mtk_container_new(int x, int y, int w, int h);
void mtk_container_add(mtk_container_t* c, mtk_widget_t* widget);

/* window.c */
mtk_window_t* mtk_window_new(int w, int h);
void mtk_window_add(mtk_window_t* window, mtk_widget_t* widget);

/* text.c */
mtk_text_t* mtk_text_new(int x, int y, int w, int h, char *t);
void mtk_text_set(mtk_text_t *widget, char *t);

/* timer.c */
void mtk_timer_add(double interval, int(*callback)(void *data), void *data);

/* utilities */
char* entityname(mpd_InfoEntity *entity);
mtk_list_t* mtk_list_new();
void mtk_list_append(mtk_list_t *l, void* d);
void mtk_list_insert(mtk_list_t *l, void* d);
void* mtk_list_replace(mtk_list_t *l, void* d);
void* mtk_list_remove(mtk_list_t *l);
void* mtk_list_goto(mtk_list_t *l, int i);
void* mtk_list_next(mtk_list_t *l);

static inline void* mtk_list_current(mtk_list_t *l) {
	return l->current_node? l->current_node->data : NULL;
}

static inline int mtk_list_length(mtk_list_t *l) {
	return l->count;
}

#define mtk_list_foreach(l,d) \
	for (mtk_list_node_t *__n = (l)->first; \
		(d)=__n?__n->data:(d), __n; \
		__n = __n->next)

void* xmalloc(size_t size);
void* xmalloc0(size_t size);

#define die_on(x, fmt, args...) \
	if (x) { \
		fprintf(stderr, "%s:%d " fmt, __FILE__, __LINE__, ## args); \
		exit(1); \
	}

#endif
