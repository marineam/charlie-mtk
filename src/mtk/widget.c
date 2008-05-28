#include <cairo.h>
#include <cairo-xcb.h>
#include <assert.h>
#include <mtk.h>

#include "private.h"

static void init(void *this, mtk_widget_t* parent)
{
	mtk_widget_t *widget = this;

	assert(parent->surface);
	widget->surface = cairo_surface_create_similar(
			parent->surface,
			CAIRO_CONTENT_COLOR,
			widget->w, widget->h);

	if (call_defined(widget,update))
		call(widget,update);

	call(widget,redraw);
}

static void draw(void* this)
{
	mtk_widget(this)->redraw = false;
}

static void redraw(void* vthis)
{
	mtk_widget_t *this = vthis;

	this->redraw = true;
	if (this->parent)
		call(this->parent,redraw);
}

static void set_coord(void *vthis, int x, int y)
{
	mtk_widget_t *this = vthis;

	this->x = x;
	this->y = y;

	/* we only need to redraw the parent for coord changes */
	if (this->parent)
		call(this->parent,redraw);
}

static void set_size(void *vthis, int w, int h)
{
	mtk_widget_t *this = vthis;

	this->w = w;
	this->h = h;

	if (this->surface && this->parent) {
		cairo_surface_destroy(this->surface);
		init(this, this->parent);
	}
}

static void get_coord(void *vthis, int *x, int *y)
{
	mtk_widget_t *this = vthis;

	if (x)
		*x = this->x;
	if (y)
		*y = this->y;
}

static void get_size(void *vthis, int *w, int *h)
{
	mtk_widget_t *this = vthis;

	if (w)
		*w = this->w;
	if (h)
		*h = this->h;
}

static void set_parent(void *this, mtk_widget_t *parent)
{
	mtk_widget(this)->parent = parent;
}

static void objfree(void* vthis)
{
	mtk_widget_t *this = vthis;

	if (this->surface)
		cairo_surface_destroy(this->surface);

	super(this,mtk_widget,free);
}

mtk_widget_t* mtk_widget_new(size_t size)
{
	mtk_widget_t* this = mtk_widget(mtk_object_new(size));
	SET_CLASS(this, mtk_widget);

	return this;
}

METHOD_TABLE_INIT(mtk_widget, mtk_object)
	_METHOD(free, objfree);
	METHOD(init);
	METHOD(draw);
	METHOD(redraw);
	METHOD(set_coord);
	METHOD(set_size);
	METHOD(get_coord);
	METHOD(get_size);
	METHOD(set_parent);
METHOD_TABLE_END
