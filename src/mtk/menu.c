#include <assert.h>
#include <cairo.h>
#include <mtk.h>

#include "private.h"

struct item {
	char *text;
	mtk_widget_t *widget;
};

static void draw(mtk_widget_t *this)
{
	cairo_t *cr;
	cairo_pattern_t *pat;
	cairo_font_extents_t fe;
	struct item *item;
	int y = 0;

	super(this,mtk_menu,mtk_widget,draw);

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

	mtk_list_foreach(mtk_menu(this)->menu, item) {
		cairo_set_source_rgb(cr, 0, 0, 0);
		cairo_move_to(cr, mtk_menu(this)->slide - this->w/2,
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

static void add_widget(mtk_container_t *c, mtk_widget_t *w)
{
	call(w,mtk_widget,set_size,mtk_widget(c)->w-UNIT,mtk_widget(c)->h);
	call(w,mtk_widget,set_coord,UNIT,0);
	super(c,mtk_menu,mtk_container,add_widget,w);
}

static void add_item(mtk_menu_t *this, mtk_widget_t *widget, char *text)
{
	struct item *item = xmalloc(sizeof(struct item));

	call(this,mtk_container,add_widget, widget);
	item->text = strdup(text);
	item->widget = widget;
	mtk_list_append(this->menu, item);
}

static void set_size(mtk_widget_t *this, int w, int h)
{
	mtk_widget_t *widget;

	/* Skip mtk_container's set_size */
	super(this,mtk_container,mtk_widget,set_size, w, h);

	mtk_list_foreach(mtk_container(this)->widgets, widget)
		call(widget,mtk_widget,set_size, w-UNIT, h);
}

static bool slider(void *data)
{
	mtk_menu_t *this = data;

	this->slide += this->slide_dir;

	call(this,mtk_widget,redraw);

	if (this->slide < 0)
		this->slide = 0;

	if (this->slide == 0 || this->slide >= mtk_widget(this)->w/2) {
		this->slide_active = false;
		return false;
	}
	else
		return true;
}

static void mouse_press(mtk_widget_t *this, int x, int y)
{
	mtk_menu_t *m = mtk_menu(this);

	if (x < m->slide) {
		int pos = y/UNIT;
		struct item *item = mtk_list_goto(m->menu, pos);

		if (item) {
			call(m,mtk_container,reorder_top,item->widget);
			m->slide_dir *= -1;
			if (!m->slide_active) {
				m->slide_active = true;
				mtk_timer_add(0.03, slider, m);
			}
		}
	}
	else if (x < m->slide + UNIT || (m->slide && x > m->slide)) {
		m->slide_dir *= -1;
		if (!m->slide_active) {
			m->slide_active = true;
			mtk_timer_add(0.03, slider, m);
		}
	}
	else
		super(m,mtk_menu,mtk_widget,mouse_press, x, y);
}

static void mouse_release(mtk_widget_t *this, int x, int y)
{
	mtk_menu_t *m = mtk_menu(this);

	if (x >= m->slide + UNIT)
		super(m,mtk_menu,mtk_widget,mouse_release, x, y);
}

static void mouse_move(mtk_widget_t *this, int x, int y)
{
	mtk_menu_t *m = mtk_menu(this);

	if (x >= m->slide + UNIT)
		super(m,mtk_menu,mtk_widget,mouse_move, x, y);
}

mtk_menu_t* mtk_menu_new(size_t size)
{
	mtk_menu_t *this = mtk_menu(mtk_container_new(size));
	SET_CLASS(this, mtk_menu);

	this->menu = mtk_list_new();
	this->slide_dir = -20;

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
