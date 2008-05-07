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

void set_geometry(mtk_widget_t *this, int x, int y, int w, int h)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
}

mtk_widget_t* mtk_widget_new(size_t size, int x, int y, int w, int h)
{
	mtk_widget_t* this = MTK_WIDGET(mtk_object_new(size));
	SET_CLASS(this, mtk_widget);
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;

	return this;
}

METHOD_TABLE_INIT(mtk_widget, mtk_object)
	METHOD(init);
	METHOD(set_geometry);
METHOD_TABLE_END
