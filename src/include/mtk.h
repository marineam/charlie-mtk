#ifndef MTK_H
#define MTK_H

#include <stdlib.h>
#include <stdio.h>

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

/* main.c */
void mtk_init();
void mtk_cleanup();
int mtk_event();

/* clickarea.c */
//mtk_ mtk_clickarea_new();

/* mpdlist.c */
//GtkWidget* mtk_mpdlist_new();

/* window.c */
mtk_window_t* mtk_window_new(int w, int h);

/* utilities */
mtk_list_t* mtk_list_new();
void mtk_list_append(mtk_list_t *l, void* d);
void* xmalloc(size_t size);
void* xmalloc0(size_t size);

#define die_on(x, fmt, args...) \
	if (x) { \
		fprintf(stderr, fmt, ## args); \
		exit(1); \
	}

#endif
