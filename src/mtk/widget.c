#include <cairo.h>
#include <cairo-xcb.h>
#include <assert.h>
#include <mtk.h>

#include "private.h"

static void init(mtk_widget_t* widget, mtk_widget_t* parent)
{
	assert(parent->surface);
	widget->surface = cairo_surface_create_similar(
			parent->surface,
			CAIRO_CONTENT_COLOR,
			widget->w, widget->h);

	if (call_defined(widget,mtk_widget,update))
		call(widget,mtk_widget,update);

	call(widget,mtk_widget,redraw);
}

static void draw(mtk_widget_t* this)
{
	this->redraw = false;
}

static void redraw(mtk_widget_t* this)
{
	this->redraw = true;
	if (this->parent)
		call(this->parent,mtk_widget,redraw);
}

static void set_coord(mtk_widget_t *this, int x, int y)
{
	this->x = x;
	this->y = y;

	/* we only need to redraw the parent for coord changes */
	if (this->parent)
		call(this->parent,mtk_widget,redraw);
}

static void set_size(mtk_widget_t *this, int w, int h)
{
	this->w = w;
	this->h = h;

	if (this->surface && this->parent) {
		cairo_surface_destroy(this->surface);
		init(this, this->parent);
	}
}

static void get_coord(mtk_widget_t *this, int *x, int *y)
{
	if (x)
		*x = this->x;
	if (y)
		*y = this->y;
}

static void get_size(mtk_widget_t *this, int *w, int *h)
{
	if (w)
		*w = this->w;
	if (h)
		*h = this->h;
}

static void set_parent(mtk_widget_t *this, mtk_widget_t *parent)
{
	this->parent = parent;
}

mtk_widget_t* mtk_widget_new(size_t size)
{
	mtk_widget_t* this = mtk_widget(mtk_object_new(size));
	SET_CLASS(this, mtk_widget);

	return this;
}

METHOD_TABLE_INIT(mtk_widget, mtk_object)
	METHOD(init);
	METHOD(draw);
	METHOD(redraw);
	METHOD(set_coord);
	METHOD(set_size);
	METHOD(get_coord);
	METHOD(get_size);
	METHOD(set_parent);
METHOD_TABLE_END
