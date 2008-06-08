#include <string.h>
#include <assert.h>
#include <cairo.h>
#include <mtk.h>
#include <libmpdclient.h>

#include "private.h"

static void draw(void *vthis)
{
	mtk_slider_t *this = vthis;
	mtk_widget_t *widget = vthis;
	cairo_t *cr = cairo_create(widget->surface);

	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_paint(cr);

	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	cairo_move_to(cr, 0, widget->h/2.0);
	cairo_line_to(cr, widget->w, widget->h/2.0);
	cairo_stroke(cr);

	cairo_move_to(cr, this->value*(widget->w-2)+1, 0);
	cairo_line_to(cr, this->value*(widget->w-2)+1, widget->h);
	cairo_stroke(cr);

	cairo_destroy(cr);

	super(widget,mtk_slider,draw);
}

static void set_value(void *vthis, double val)
{
	mtk_slider_t *this = vthis;

	this->value = val;
	call(this,redraw);
}

mtk_slider_t* mtk_slider_new(size_t size, double val)
{
	mtk_slider_t *this = mtk_slider(mtk_widget_new(size));
	SET_CLASS(this, mtk_slider);

	this->value = val;
	return this;
}

METHOD_TABLE_INIT(mtk_slider, mtk_widget)
	METHOD(set_value);
	METHOD(draw);
METHOD_TABLE_END
