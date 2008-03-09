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
