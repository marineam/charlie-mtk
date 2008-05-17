#include <mtk.h>

#include "private.h"

struct item {
	mtk_widget_t *widget;
	int size;
};

static void repack(mtk_hpack_t *this)
{
	struct item *i;
	int size, resize, offset = 0, resizeable = 0;

	size = mtk_widget(this)->w;

	mtk_list_foreach(this->order, i) {
		if (i->size)
			size -= i->size;
		else
			resizeable += 1;
	}

	if (size < 0) {
		warn("not enough room in pack, resizing all widgets!");
		resizeable = mtk_list_length(this->order);
	}

	if (resizeable)
		resize = size / resizeable;

	mtk_list_foreach(this->order, i) {
		int newsize;

		if (i->size && size >= 0)
			newsize = i->size;
		else
			newsize = resize;

		call(i->widget,mtk_widget,set_coord, offset, 0);
		call(i->widget,mtk_widget,set_size,
				newsize, mtk_widget(this)->h);

		offset += newsize;
	}
}

void pack_left(mtk_hpack_t *this, mtk_widget_t *widget, int w)
{
	struct item *item = xmalloc(sizeof(struct item));

	item->widget = widget;
	item->size = w;
	mtk_list_prepend(this->order, item);

	super(this,mtk_hpack,mtk_container,add_widget,widget);

	repack(this);
}

void pack_right(mtk_hpack_t *this, mtk_widget_t *widget, int w)
{
	struct item *item = xmalloc(sizeof(struct item));

	item->widget = widget;
	item->size = w;
	mtk_list_append(this->order, item);

	super(this,mtk_hpack,mtk_container,add_widget,widget);

	repack(this);
}

mtk_hpack_t* mtk_hpack_new(size_t size)
{
	mtk_hpack_t *this = mtk_hpack(mtk_container_new(size));
	SET_CLASS(this, mtk_hpack);

	this->order = mtk_list_new();

	return this;
}

METHOD_TABLE_INIT(mtk_hpack, mtk_container)
	METHOD(pack_left);
	METHOD(pack_right);
METHOD_TABLE_END
