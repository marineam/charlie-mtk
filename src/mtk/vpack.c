#include <mtk.h>

#include "private.h"

struct item {
	mtk_widget_t *widget;
	int size;
};

static void repack(mtk_vpack_t *this)
{
	struct item *i;
	int size, resize, offset = 0, resizeable = 0;

	size = mtk_widget(this)->h;

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

	resize = mtk_widget(this)->h / resizeable;

	mtk_list_foreach(this->order, i) {
		int newsize;

		if (i->size && size >= 0)
			newsize = i->size;
		else
			newsize = resize;

		call(i->widget,mtk_widget,set_coord, 0, offset);
		call(i->widget,mtk_widget,set_size,
				mtk_widget(this)->w, newsize);

		offset += newsize;
	}
}

void pack_top(mtk_vpack_t *this, mtk_widget_t *widget, int h)
{
	struct item *item = xmalloc(sizeof(struct item));

	item->widget = widget;
	item->size = h;
	mtk_list_prepend(this->order, item);

	super(this,mtk_hpack,mtk_container,add_widget,widget);

	repack(this);
}

void pack_bottom(mtk_vpack_t *this, mtk_widget_t *widget, int h)
{
	struct item *item = xmalloc(sizeof(struct item));

	item->widget = widget;
	item->size = h;
	mtk_list_append(this->order, item);

	super(this,mtk_hpack,mtk_container,add_widget,widget);

	repack(this);
}

mtk_vpack_t* mtk_vpack_new(size_t size)
{
	mtk_vpack_t *this = mtk_vpack(mtk_container_new(size));
	SET_CLASS(this, mtk_vpack);

	this->order = mtk_list_new();

	return this;
}

METHOD_TABLE_INIT(mtk_vpack, mtk_container)
	METHOD(pack_top);
	METHOD(pack_bottom);
METHOD_TABLE_END
