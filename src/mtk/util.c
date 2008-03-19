#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <mtk.h>

mtk_list_t* mtk_list_new()
{
	mtk_list_t *l = xmalloc0(sizeof(mtk_list_t));
	return l;
}

void mtk_list_append(mtk_list_t *l, void* d)
{
	l->current_node = NULL;
	l->current_index = l->count;
	mtk_list_insert(l,d);
}

void mtk_list_insert(mtk_list_t *l, void* d)
{
	mtk_list_node_t *n = xmalloc0(sizeof(mtk_list_node_t));
	n->data = d;

	if (l->current_index == 0) {
		assert(l->current_node == l->first);
		if (l->first) {
			n->next = l->first;
			l->first->prev = n;
		}
		l->first = n;
	}
	else if (l->current_index == l->count) {
		assert(l->current_node == NULL);
		if (l->last) {
			n->prev = l->last;
			l->last->next = n;
		}
		l->last = n;
	}
	else {
		assert(l->current_node && l->current_node->prev);
		n->prev = l->current_node->prev;
		n->next = l->current_node;
		n->prev->next = n;
		n->next->prev = n;
	}

	l->current_node = n;
	l->count++;
}

void* mtk_list_replace(mtk_list_t *l, void* d)
{
	void *data;
	data = mtk_list_remove(l);
	mtk_list_insert(l,d);
	return data;
}

void* mtk_list_remove(mtk_list_t *l)
{
	mtk_list_node_t *old;
	void *data;

	old = l->current_node;
	if (l->current_index == 0) {
		assert(l->current_node == l->first);
		l->first = l->first->next;
		l->current_node = l->first;
		if (l->first)
			l->first->prev = NULL;
	}
	else if (l->current_index == l->count) {
		assert(l->current_node == NULL);
		/* this node doesn't exist, nothing to do */
	}
	else {
		assert(l->current_node && l->current_node->prev);
		l->current_node->prev->next = l->current_node->next;
		if (l->current_node->next)
			l->current_node->next->prev = l->current_node->prev;
		l->current_node = l->current_node->next;
	}

	data = old->data;
	free(old);
	l->count--;

	return data;
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

	assert(l->current_node || l->current_index == l->count);

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
