#ifndef MTK_H
#define MTK_H

#include <stdlib.h>
#include <stdio.h>

#include "config.h"

typedef struct mtk_list_node {
	struct mtk_list_node *next;
	struct mtk_list_node *prev;
	void* data;
} mtk_list_node_t;

typedef struct mtk_list {
	struct mtk_list_node *first;
	struct mtk_list_node *last;
	int count;
} mtk_list_t;

typedef struct mtk_window mtk_window_t;
typedef struct mtk_widget mtk_widget_t;

/* main.c */
void mtk_init();
void mtk_cleanup();
int mtk_event();

/* clickarea.c */
mtk_widget_t* mtk_clickarea_new(int x, int y, int w, int h);

/* mpdlist.c */
mtk_widget_t* mtk_mpdlist_new(int x, int y, int w, int h, mtk_list_t *list);

/* window.c */
mtk_window_t* mtk_window_new(int w, int h);
void mtk_window_add(mtk_window_t* window, mtk_widget_t* widget);

/* utilities */
mtk_list_t* mtk_list_new();
void mtk_list_append(mtk_list_t *l, void* d);

static inline int mtk_list_length(mtk_list_t *l) {
	return l->count;
}

#define mtk_list_foreach(l,d) \
	for (mtk_list_node_t *n = (l)->first; \
		(d)=n?n->data:(d), n; \
		n = n->next)

void* xmalloc(size_t size);
void* xmalloc0(size_t size);

#define die_on(x, fmt, args...) \
	if (x) { \
		fprintf(stderr, fmt, ## args); \
		exit(1); \
	}

#endif
