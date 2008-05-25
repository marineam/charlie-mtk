#include <assert.h>
#include <cairo.h>
#include <mtk.h>

#include "private.h"

struct item {
	char *text;
	mtk_widget_t *widget;
};

static void draw(void *vthis)
{
	mtk_widget_t *this = vthis;

	cairo_t *cr;
	cairo_pattern_t *pat;
	cairo_font_extents_t fe;
	struct item *item;
	int y = 0;

	super(this,mtk_menu,draw);

	cr = cairo_create(this->surface);
	pat = cairo_pattern_create_linear(mtk_menu(this)->slide, 0,
		mtk_menu(this)->slide+UNIT, 0);
	cairo_pattern_add_color_stop_rgb(pat, 1.0, 0.6, 0.6, 0.9);
	cairo_pattern_add_color_stop_rgb(pat, 0.0, 1.0, 1.0, 1.0);
	cairo_rectangle(cr, mtk_menu(this)->slide, 0, UNIT, this->h);
	cairo_set_source(cr, pat);
	cairo_fill(cr);
	cairo_pattern_destroy(pat);

	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_rectangle(cr, 0, 0, mtk_menu(this)->slide, this->h);
	cairo_fill(cr);

	cairo_select_font_face(cr, "Sans",
		CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, UNIT);
	cairo_font_extents(cr, &fe);
	/* adjust font size so UNIT == fe.height */
	cairo_set_font_size(cr, UNIT * (UNIT/fe.height));
	cairo_font_extents(cr, &fe);

	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	mtk_list_foreach(mtk_menu(this)->menu, item) {
		if (mtk_menu(this)->top == item->widget) {
			cairo_set_source_rgb(cr, 0.8, 0.9, 1.0);
			cairo_rectangle(cr, 0, y, mtk_menu(this)->slide, UNIT);
			cairo_fill(cr);
			cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
		}
		cairo_move_to(cr,
			mtk_menu(this)->slide - mtk_menu(this)->slide_max,
			y + fe.ascent);
		cairo_show_text(cr, item->text);
		y += UNIT;
	}

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

static void add_widget(void *this, mtk_widget_t *w)
{
	mtk_widget_t *c = this;

	if (!mtk_menu(this)->top) {
		mtk_menu(this)->top = w;
		mtk_menu(this)->old = w;
	}

	call(w,set_size, c->w-UNIT, c->h);
	call(w,set_coord, UNIT, 0);
	super(c,mtk_menu,add_widget, w);
}

static void add_item(void *vthis, mtk_widget_t *widget, char *text)
{
	mtk_menu_t *this = vthis;
	struct item *item = xmalloc(sizeof(struct item));
	cairo_t *cr;
	cairo_text_extents_t te;
	int max = 0;

	call(this,add_widget, widget);
	item->text = strdup(text);
	item->widget = widget;
	mtk_list_append(this->menu, item);

	/* Don't actually draw anything, just calc the max menu width */
	cr = cairo_create(mtk_widget(this)->surface);
	cairo_select_font_face(cr, "Sans",
		CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, UNIT);


	mtk_list_foreach(this->menu, item) {
		cairo_text_extents(cr, item->text, &te);
		if (te.width > max)
			max = te.width;
	}

	assert(max + UNIT <= mtk_widget(this)->w);
	this->slide_max = max;

	cairo_destroy(cr);
}

static void set_size(void *vthis, int w, int h)
{
	mtk_container_t *this = vthis;
	mtk_widget_t *widget;

	/* Skip mtk_container's set_size */
	super(this,mtk_container,set_size, w, h);

	mtk_list_foreach(this->widgets, widget)
		call(widget,set_size, w-UNIT, h);
}

static bool slider(void *data)
{
	mtk_menu_t *this = data;
	int menu_rate, item_rate;

	/* find a rate so both finish sliding at the same time
	 * use 8 frames, that seems to give a nice speed */
	menu_rate = this->slide_dir * this->slide_max/8.0;
	item_rate = -this->slide_dir *
		(mtk_widget(this)->w-this->slide_max)/8.0;

	if (this->slide_item > UNIT)
		this->slide_item += item_rate;

	if (this->slide_item < UNIT)
		this->slide_item = UNIT;

	this->slide += menu_rate;

	if (this->slide > this->slide_max)
		this->slide = this->slide_max;
	else if (this->slide < 0)
		this->slide = 0;

	assert(this->old);
	call(this->old,set_coord, this->slide_item, 0);
	call(this,redraw);

	/* detect if both sides are done sliding and finish */
	if ((this->slide == 0 && this->slide_item > mtk_widget(this)->w) ||
	    (this->slide == this->slide_max && this->slide_item == UNIT)) {
		call(this,reorder_top, this->top);
		this->old = this->top;
		this->slide_item = UNIT;
		this->slide_active = false;
		return false;
	}
	else
		return true;
}

static void mouse_press(void *this, int x, int y)
{
	mtk_menu_t *m = this;

	if (x < m->slide) {
		int pos = y/UNIT;
		struct item *item = mtk_list_goto(m->menu, pos);

		if (item && m->top != item->widget) {
			m->old = m->top;
			m->top = item->widget;
			m->slide_item = UNIT+1;
			call(m->top,set_coord, UNIT, 0);
			call(m->old,set_coord, m->slide_item, 0);
			/* FIXME: can we do the following in one call? */
			call(m,reorder_top, m->top);
			call(m,reorder_top, m->old);
		}
	}

	if (x < m->slide + UNIT || (m->slide && x > m->slide)) {
		m->slide_dir *= -1;
		if (!m->slide_active) {
			m->slide_active = true;
			/* 30 frames per second */
			mtk_timer_add(1.0/30, slider, m);
		}
	}
	else
		super(m,mtk_menu,mouse_press, x, y);
}

static void mouse_release(void *this, int x, int y)
{
	mtk_menu_t *m = this;

	if (x >= m->slide + UNIT)
		super(m,mtk_menu,mouse_release, x, y);
}

static void mouse_move(void *this, int x, int y)
{
	mtk_menu_t *m = this;

	if (x >= m->slide + UNIT)
		super(m,mtk_menu,mouse_move, x, y);
}

mtk_menu_t* mtk_menu_new(size_t size)
{
	mtk_menu_t *this = mtk_menu(mtk_container_new(size));
	SET_CLASS(this, mtk_menu);

	this->menu = mtk_list_new();
	this->slide_dir = -1;

	return this;
}

METHOD_TABLE_INIT(mtk_menu, mtk_container)
	METHOD(draw);
	METHOD(set_size);
	METHOD(mouse_press);
	METHOD(mouse_release);
	METHOD(mouse_move);
	METHOD(add_widget);
	METHOD(add_item);
METHOD_TABLE_END
