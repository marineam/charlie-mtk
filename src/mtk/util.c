#include <stdlib.h>
#include <string.h>
#include <mtk.h>

mtk_list_t* mtk_list_new()
{
	mtk_list_t *l = xmalloc0(sizeof(mtk_list_t));
	return l;
}

void mtk_list_append(mtk_list_t *l, void* d)
{
	mtk_list_node_t *n = xmalloc0(sizeof(mtk_list_node_t));
	n->data = d;
	if (!l->first)
		l->first = n;
	if (l->last)
		l->last->next = n;
	l->last = n;
	l->count++;
}

void* mtk_list_goto(mtk_list_t *l, int i)
{
	if (!l->current_node || i < l->current_index) {
		l->current_index = 0;
		l->current_node = l->first;
	}

	while (l->current_node && l->current_index < i) {
		l->current_index++;
		l->current_node = l->current_node->next;
	}

	return l->current_node ? l->current_node->data : NULL;
}

void* mtk_list_next(mtk_list_t *l)
{
	if (!l->current_node)
		return NULL;

	l->current_index++;
	l->current_node = l->current_node->next;

	return l->current_node ? l->current_node->data : NULL;
}

void* xmalloc(size_t size)
{
	void *ret = malloc(size);
	die_on(!ret, "malloc failed\n");
	return ret;
}

void* xmalloc0(size_t size)
{
	void *ret = xmalloc(size);
	memset(ret, 0, size);
	return ret;
}
