#include <string.h>
#include <assert.h>
#include <cairo.h>
#include <mtk.h>
#include <libmpdclient.h>

#include "private.h"

static void draw(void *this)
{
	mtk_widget_t *widget = this;
	mtk_text_list_t *text_list = this;
	cairo_t *cr;
	cairo_pattern_t *pat;
	int start, y;
	void *item;

	cr = cairo_create(widget->surface);

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 0, 0, widget->w, widget->h);
	cairo_fill(cr);

	start = text_list->scroll_top / UNIT;
	y = UNIT - text_list->scroll_top % UNIT;

	for (item = mtk_list_goto(text_list->list, start);
	     item && y < widget->h - UNIT;
	     item = mtk_list_next(text_list->list)) {
		call(text_list,_item_draw, cr, item, y);
		y += UNIT;
	}

	pat = cairo_pattern_create_linear(0, 0, 0, UNIT);
	cairo_pattern_add_color_stop_rgb(pat, 0.0, 0.6, 0.6, 0.9);
	cairo_pattern_add_color_stop_rgb(pat, 1.0, 1.0, 1.0, 1.0);
	cairo_rectangle(cr, 0, 0, widget->w, UNIT);
	cairo_set_source(cr, pat);
	cairo_fill(cr);
	cairo_pattern_destroy(pat);

	pat = cairo_pattern_create_linear(0, widget->h-UNIT, 0, widget->h);
	cairo_pattern_add_color_stop_rgb(pat, 1.0, 0.6, 0.6, 0.9);
	cairo_pattern_add_color_stop_rgb(pat, 0.0, 1.0, 1.0, 1.0);
	cairo_rectangle(cr, 0, widget->h - UNIT, widget->w, widget->h);
	cairo_set_source(cr, pat);
	cairo_fill(cr);
	cairo_pattern_destroy(pat);

	cairo_move_to(cr, widget->w/2.0-UNIT*0.25, UNIT*0.75);
	cairo_line_to(cr, widget->w/2.0, UNIT*0.25);
	cairo_line_to(cr, widget->w/2.0+UNIT*0.25, UNIT*0.75);
	cairo_close_path(cr);

	if (text_list->scroll_top == 0)
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.9, 0.2);
	else if (text_list->scroll_dir < 0)
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.9, 0.9);
	else
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.9, 0.7);
	cairo_fill(cr);

	cairo_move_to(cr, widget->w/2.0-UNIT*0.25, widget->h - UNIT*0.75);
	cairo_line_to(cr, widget->w/2.0, widget->h - UNIT*0.25);
	cairo_line_to(cr, widget->w/2.0+UNIT*0.25, widget->h - UNIT*0.75);
	cairo_close_path(cr);

	if (text_list->scroll_top ==
		mtk_list_length(text_list->list)*UNIT - widget->h + 2*UNIT)
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.9, 0.2);
	else if (text_list->scroll_dir > 0)
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.9, 0.9);
	else
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.9, 0.7);
	cairo_fill(cr);

	cairo_destroy(cr);

	super(widget,mtk_text_list,draw);
}

/* compute new scroll position */
static void scroll(mtk_text_list_t *text_list)
{
	/* extreme scroll positions, max is the top, min bottom */
	int min = 0;
	int max = (mtk_list_length(text_list->list) * UNIT) -
		(mtk_widget(text_list)->h - 2*UNIT);

	if (max <= min) {
		text_list->scroll_dir = 0;
		text_list->scroll_top = 0;
		return;
	}

	if (!text_list->scroll_hold) {
		int diff;

		if (text_list->scroll_dir >= 0)
			diff = (UNIT - text_list->scroll_top % UNIT) % UNIT;
		else
			diff = -text_list->scroll_top % UNIT;

		if (diff == 0)
			text_list->scroll_dir = 0;
		else {
			if (abs(text_list->scroll_dir) > abs(diff))
				text_list->scroll_dir = diff*0.9;
			else
				text_list->scroll_dir *= 0.9;

			if (!text_list->scroll_dir)
				text_list->scroll_dir = diff>0?1:-1;
		}
	}

	text_list->scroll_top += text_list->scroll_dir;

	if (text_list->scroll_top > max) {
		text_list->scroll_top = max;
		text_list->scroll_dir = 0;
	}
	else if (text_list->scroll_top < min) {
		text_list->scroll_top = min;
		text_list->scroll_dir = 0;
	}
}

static bool timed_draw(void *data)
{
	mtk_text_list_t *text_list = data;

	scroll(text_list);
	call(text_list,redraw);

	if (!text_list->scroll_dir) {
		text_list->scroll_active = false;
		return false;
	}
	else
		return true;
}

static void mouse_press(void *this, int x, int y)
{
	mtk_widget_t *widget = this;
	mtk_text_list_t *text_list = this;

	if (y <= UNIT || y > widget->h-UNIT) {
		text_list->scroll_hold = true;
		text_list->scroll_dir = y<=UNIT ? -UNIT/4 : UNIT/4;
		if (!text_list->scroll_active) {
			text_list->scroll_active = true;
			mtk_timer_add(1.0/30, timed_draw, text_list);
		}
	}
	else {
		int pos = (float)(y - UNIT) / UNIT +
			(float)text_list->scroll_top / UNIT;

		if (pos < mtk_list_length(text_list->list)) {
			call(text_list,_item_click,
				mtk_list_goto(text_list->list, pos));
		}
	}

	scroll(text_list);
	call(text_list,redraw);
}

static void mouse_release(void *this, int x, int y)
{
	mtk_text_list_t *text_list = this;

	text_list->scroll_hold = false;
}

static char* _item_text(void *this, void *item)
{
	return item;
}

static void _item_draw(void *vthis, cairo_t *cr, void *item, int y)
{
	mtk_text_list_t *this = vthis;
	cairo_text_extents_t te;

	cairo_select_font_face(cr, "Sans",
		CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, UNIT*0.5);
	/* font_extents seems like the "correct" way to do this but
	 * the font's ascent value doesn't actually match it's ascent */
	cairo_text_extents(cr, "M", &te);

	cairo_save(cr);
	cairo_rectangle(cr, 0, y, mtk_widget(this)->w, UNIT);
	cairo_clip(cr);
	cairo_set_source_surface(cr, this->item_background, 0, y);
	cairo_paint(cr);
	cairo_restore(cr);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_move_to(cr, UNIT*0.2, y+UNIT*0.5 + te.height*0.5);
	cairo_show_text(cr, call(this,_item_text, item));
}

static void _item_click(void *this, void *item)
{
}

static void _item_free(void *this, void *item)
{
	free(item);
}

static void cache_background(mtk_text_list_t *this)
{
	int w = mtk_widget(this)->w;

	if (this->item_background)
		cairo_surface_destroy(this->item_background);

	if (mtk_widget(this)->surface) {
		cairo_pattern_t *pat;
		cairo_t *cr;

		this->item_background = cairo_surface_create_similar(
			mtk_widget(this)->surface,
			CAIRO_CONTENT_COLOR, w, UNIT);

		cr = cairo_create(this->item_background);

		cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
		cairo_rectangle(cr, 0, 0, w, UNIT);
		cairo_fill(cr);

		pat = cairo_pattern_create_linear(0, 0, w, 0);
		cairo_pattern_add_color_stop_rgb(pat, 0.0, 0.8, 0.9, 0.9);
		cairo_pattern_add_color_stop_rgb(pat, 0.6, 1.0, 1.0, 1.0);

		cairo_rectangle(cr, UNIT*0.1, UNIT*0.1, w, UNIT*0.8);
		cairo_set_source(cr, pat);
		cairo_fill(cr);

		cairo_pattern_destroy(pat);
		cairo_destroy(cr);
	}
}

static void set_size(void *vthis, int w, int h)
{
	mtk_text_list_t *this = vthis;

	super(this,mtk_text_list,set_size, w, h);
	cache_background(this);
}

static void init(void *vthis, mtk_widget_t *parent)
{
	mtk_text_list_t *this = vthis;

	super(this,mtk_text_list,init, parent);
	cache_background(this);
}

static void set_list(void *vthis, mtk_list_t *list)
{
	mtk_text_list_t *this = vthis;
	void *item;

	mtk_list_foreach(this->list, item)
		call(this,_item_free, item);

	mtk_list_free(this->list);

	this->list = list;
	this->scroll_dir = 0;
	this->scroll_top = 0;
	call(this,redraw);
}

static void objfree(void *vthis)
{
	mtk_text_list_t *this = vthis;
	void *item;

	if (this->item_background)
		cairo_surface_destroy(this->item_background);

	mtk_list_foreach(this->list, item)
		call(this,_item_free, item);

	mtk_list_free(this->list);

	super(this,mtk_text_list,free);
}

mtk_text_list_t* mtk_text_list_new(size_t size, mtk_list_t *list)
{
	mtk_text_list_t *this = mtk_text_list(mtk_widget_new(size));

	SET_CLASS(this, mtk_text_list);

	if (list)
		this->list = list;
	else
		this->list = mtk_list_new();

	return this;
}

METHOD_TABLE_INIT(mtk_text_list, mtk_widget)
	_METHOD(free, objfree);
	METHOD(draw);
	METHOD(mouse_press);
	METHOD(mouse_release);
	METHOD(set_size);
	METHOD(set_list);
	METHOD(init);
	METHOD(_item_text);
	METHOD(_item_draw);
	METHOD(_item_click);
	METHOD(_item_free);
METHOD_TABLE_END
