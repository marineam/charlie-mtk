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

	if (widget->update)
		widget->update(widget);

	assert(widget->draw);
	widget->draw(widget);
}

void _mtk_widget_new(mtk_widget_t *widget, int x, int y, int w, int h)
{
	widget->x = x;
	widget->y = y;
	widget->w = w;
	widget->h = h;
	widget->init = init;
}

