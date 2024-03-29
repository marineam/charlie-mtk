
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <mtk.h>
#include "private.h"

struct event {
	void *data;
	void (*callback)(void *data);
};

static mtk_list_t *events;

void _mtk_event_init()
{
	events = mtk_list_new();
}

bool _mtk_event()
{
	struct event *e;

	mtk_list_goto(events, 0);
	if ((e = mtk_list_remove(events))) {
		e->callback(e->data);
		free(e);
		return true;
	}
	else
		return false;
}

void mtk_event_add(void(*callback)(void *data), void *data)
{
	struct event *e = xmalloc(sizeof(struct event));
	assert(callback);

	e->callback = callback;
	e->data = data;
	mtk_list_append(events, e);
}
