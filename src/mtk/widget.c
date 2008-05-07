#include <string.h>
#include <assert.h>
#include <cairo.h>
#include <mtk.h>
#include <libmpdclient.h>

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

	assert(call_defined(widget,mtk_widget,draw));
	call(widget,mtk_widget,draw);
}

static void draw(mtk_widget_t* this)
{
	/* do nothing, this function should be overridden to do that
	 * however we do want to redraw our parent */
	if (this->parent)
		call(this->parent,mtk_widget,draw);
}

static void set_geometry(mtk_widget_t *this, int x, int y, int w, int h)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
}

static void set_parent(mtk_widget_t *this, mtk_widget_t *parent)
{
	this->parent = parent;
}

mtk_widget_t* mtk_widget_new(size_t size, int x, int y, int w, int h)
{
	mtk_widget_t* this = mtk_widget(mtk_object_new(size));
	SET_CLASS(this, mtk_widget);
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;

	return this;
}

METHOD_TABLE_INIT(mtk_widget, mtk_object)
	METHOD(init);
	METHOD(draw);
	METHOD(set_geometry);
	METHOD(set_parent);
METHOD_TABLE_END
