#ifndef MTK_UTIL_H
#define MTK_UTIL_H

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

/* little mpd helper thingy, move out of mtk some time */
#include <libmpdclient.h>
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
