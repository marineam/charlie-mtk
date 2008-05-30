#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <mtk.h>


#ifndef NDEBUG
static void list_audit(mtk_list_t *l) {
	void *d;
	int i;

	if (l->count == 0) {
		assert(l->first == NULL);
		assert(l->last == NULL);
		assert(l->current_node == NULL);
		assert(l->current_index == 0);
		return;
	}

	assert(l->first);
	assert(l->last);

	i = 0;
	mtk_list_foreach(l, d) {
		assert(d);
		if (l->current_node == __n)
			assert(l->current_index == i);
		i++;
	}

	assert(i == l->count);
}
#else
 #define list_audit(l)
#endif

mtk_list_t* mtk_list_new()
{
	mtk_list_t *l = xmalloc0(sizeof(mtk_list_t));
	list_audit(l);
	return l;
}

void mtk_list_free(mtk_list_t *l)
{
	mtk_list_node_t *curr, *next;

	list_audit(l);
	for (curr = l->first; curr; curr = next) {
		next = curr->next;
		free(curr);
	}
	free(l);
}

void mtk_list_free_obj(mtk_list_t *l)
{
	mtk_list_node_t *curr, *next;

	list_audit(l);
	for (curr = l->first; curr; curr = next) {
		next = curr->next;
		call(mtk_object(curr->data), free);
		free(curr);
	}
	free(l);
}

void mtk_list_append(mtk_list_t *l, void* d)
{
	l->current_node = NULL;
	l->current_index = l->count;
	mtk_list_insert(l,d);
}

void mtk_list_prepend(mtk_list_t *l, void* d)
{
	l->current_node = l->first;
	l->current_index = 0;
	mtk_list_insert(l,d);
}

void mtk_list_insert(mtk_list_t *l, void* d)
{
	mtk_list_node_t *n = xmalloc0(sizeof(mtk_list_node_t));

	assert(d);
	n->data = d;

	if (l->current_index == 0) {
		assert(l->current_node == l->first);
		if (l->first) {
			n->next = l->first;
			l->first->prev = n;
		}
		l->first = n;
		if (!l->last)
			l->last = n;
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
	list_audit(l);
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
	void *data = NULL;

	old = l->current_node;
	if (l->current_index == 0) {
		assert(l->current_node == l->first);
		if (l->first == l->last)
			l->last = NULL;
		if (l->first)
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
		else
			l->last = l->current_node->prev;
		l->current_node = l->current_node->next;
	}

	if (old) {
		data = old->data;
		free(old);
		l->count--;
	}

	list_audit(l);
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

	list_audit(l);
	return l->current_node ? l->current_node->data : NULL;
}

void* mtk_list_next(mtk_list_t *l)
{
	if (!l->current_node)
		return NULL;

	l->current_index++;
	l->current_node = l->current_node->next;

	list_audit(l);
	return l->current_node ? l->current_node->data : NULL;
}

bool mtk_list_contains(mtk_list_t *l, void *data)
{
	void *d;

	mtk_list_foreach(l, d) {
		if (data == d)
			return true;
	}

	return false;
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
