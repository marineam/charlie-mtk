#include <assert.h>
#include <cairo.h>
#include <mtk.h>

#include "private.h"

CLASS(_slider, mtk_widget)
METHODS(_slider, mtk_widget)
END

static void slider_draw(mtk_widget_t *this)
{
	cairo_t *cr = cairo_create(this->surface);
	cairo_pattern_t *pat;

	pat = cairo_pattern_create_linear(0, 0, this->w, 0);
	cairo_pattern_add_color_stop_rgb(pat, 0.0, 0.6, 0.6, 0.9);
	cairo_pattern_add_color_stop_rgb(pat, 1.0, 1.0, 1.0, 1.0);
	cairo_rectangle(cr, 0, 0, this->w, this->h);
	cairo_set_source(cr, pat);
	cairo_fill(cr);
	cairo_pattern_destroy(pat);

	/*cairo_move_to(cr, widget->w/2.0-UNIT*0.25, UNIT*0.75);
	cairo_line_to(cr, widget->w/2.0, UNIT*0.25);
	cairo_line_to(cr, widget->w/2.0+UNIT*0.25, UNIT*0.75);
	cairo_close_path(cr);

	if (mpdlist->scroll_top == 0)
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.9, 0.2);
	else if (mpdlist->scroll_top > mpdlist->timed_scroll)
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.9, 0.9);
	else
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.9, 0.7);
	cairo_fill(cr);*/

	cairo_destroy(cr);
}

static void add_widget(mtk_container_t *c, mtk_widget_t *w)
{
	mtk_viewer_t *v = mtk_viewer(c);

	if (!v->base) {
		v->base = w;
		call(w,mtk_widget,set_size,mtk_widget(c)->w,mtk_widget(c)->h);
		super(c,mtk_viewer,mtk_container,add_widget,w);
		call(c,mtk_container,reorder_top, w);
	}
	else {
		call(w,mtk_widget,set_size,mtk_widget(c)->w-UNIT,mtk_widget(c)->h);
		call(w,mtk_widget,set_coord,UNIT,0);
		super(c,mtk_viewer,mtk_container,add_widget,w);
	}
}

static void set_size(mtk_widget_t *this, int w, int h)
{
	super(this,mtk_viewer,mtk_widget,set_size, w, h);
	call(mtk_viewer(this)->slider,mtk_widget,set_size, UNIT, h);
}

static void slide_in(mtk_viewer_t *this, mtk_widget_t *w)
{
	call(this,mtk_container,reorder_top, w);
	if (w != this->base)
		call(this,mtk_container,reorder_top, this->slider);
	call(this,mtk_widget,redraw);
}

_slider_t* _slider_new(size_t size)
{
	_slider_t *this = _slider(mtk_widget_new(size));
	SET_CLASS(this, _slider);
	return this;
}

mtk_viewer_t* mtk_viewer_new(size_t size)
{
	mtk_viewer_t *this = mtk_viewer(mtk_container_new(size));
	SET_CLASS(this, mtk_viewer);

	this->slider = mtk_widget(new(_slider));
	super(this,mtk_viewer,mtk_container,add_widget, this->slider);

	return this;
}

METHOD_TABLE_INIT(_slider, mtk_widget)
	_METHOD(draw, slider_draw);
METHOD_TABLE_END

METHOD_TABLE_INIT(mtk_viewer, mtk_container)
	METHOD(set_size);
	METHOD(slide_in);
	//METHOD(slide_out);
	METHOD(add_widget);
	__slider_class_init();
METHOD_TABLE_END
