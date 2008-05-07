#include <string.h>
#include <assert.h>
#include <cairo.h>
#include <mtk.h>
#include <libmpdclient.h>

#include "private.h"

static void update(mtk_widget_t *widget)
{
	mtk_mpdlist_t *mpdlist = (mtk_mpdlist_t*)widget;
	cairo_t *cr = cairo_create(mpdlist->scroll_surface);
	cairo_pattern_t *pat;
	cairo_text_extents_t te;
	mpd_InfoEntity *entity;
	int y = 0;
	char *name;

	if (mpdlist->updatelist)
		mpdlist->updatelist(mpdlist->list, mpdlist->data);

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 0, 0, WIDTH, 4000);
	cairo_fill(cr);

	cairo_set_line_width(cr, 2);
	cairo_select_font_face(cr, "Sans",
		CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, UNIT*0.5);
	/* font_extents seems like the "correct" way to do this but
	 * the font's ascent value doesn't actually match it's ascent */
	cairo_text_extents(cr, "M", &te);

	pat = cairo_pattern_create_linear(0, 0, widget->w, 0);
	cairo_pattern_add_color_stop_rgb(pat, 0.0, 0.8, 0.9, 0.9);
	cairo_pattern_add_color_stop_rgb(pat, 0.6, 1.0, 1.0, 1.0);

	mtk_list_foreach(mpdlist->list, entity) {

		cairo_rectangle(cr, UNIT*0.1, y+UNIT*0.1, widget->w, UNIT*0.8);
		cairo_set_source(cr, pat);
		cairo_fill(cr);

		cairo_set_source_rgb(cr, 0, 0, 0);

		name = entityname(entity);
		cairo_move_to(cr, UNIT*0.2, y+UNIT*0.5 + te.height*0.5);
		cairo_show_text(cr, name);
		free(name);

		y += UNIT;
	}

	cairo_pattern_destroy(pat);
	cairo_destroy(cr);
}

static void draw(mtk_widget_t *widget)
{
	mtk_mpdlist_t *mpdlist = (mtk_mpdlist_t*)widget;
	cairo_t *cr;
	cairo_pattern_t *pat;

	cr = cairo_create(widget->surface);

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

	if (mpdlist->scroll_top == 0)
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.9, 0.2);
	else if (mpdlist->scroll_top > mpdlist->timed_scroll)
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.9, 0.9);
	else
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.9, 0.7);
	cairo_fill(cr);

	cairo_move_to(cr, widget->w/2.0-UNIT*0.25, widget->h - UNIT*0.75);
	cairo_line_to(cr, widget->w/2.0, widget->h - UNIT*0.25);
	cairo_line_to(cr, widget->w/2.0+UNIT*0.25, widget->h - UNIT*0.75);
	cairo_close_path(cr);

	if (mpdlist->scroll_top ==
		mtk_list_length(mpdlist->list)*UNIT - widget->h + 2*UNIT)
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.9, 0.2);
	else if (mpdlist->scroll_top < mpdlist->timed_scroll)
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.9, 0.9);
	else
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.9, 0.7);
	cairo_fill(cr);

	cairo_rectangle(cr, 0, UNIT, widget->w, widget->h - 2*UNIT);
	cairo_clip(cr);
	cairo_set_source_surface(cr, mpdlist->scroll_surface, 0,
		-mpdlist->scroll_top + UNIT);
	cairo_paint(cr);

	cairo_destroy(cr);
}

/* cleans things up after mouse events */
static void scroll_fixup(mtk_mpdlist_t *mpdlist)
{
	/* extreme scroll positions, max is the top, min bottom */
	int min = 0;
	int max = (mtk_list_length(mpdlist->list) * UNIT) -
		(mtk_widget(mpdlist)->h - 2*UNIT);

	if (max <= min) {
		mpdlist->timed_scroll = 0;
		mpdlist->scroll_top = 0;
		return;
	}

	if (mpdlist->timed_scroll > max)
		mpdlist->timed_scroll = max;
	else if (mpdlist->timed_scroll < min)
		mpdlist->timed_scroll = min;

	if (mpdlist->scroll_top > max)
		mpdlist->scroll_top = max;
	else if (mpdlist->scroll_top < min)
		mpdlist->scroll_top = min;

	if (mpdlist->timed_scroll != mpdlist->scroll_top) {
		if (abs(mpdlist->timed_scroll - mpdlist->scroll_top) <=2)
			mpdlist->scroll_top = mpdlist->timed_scroll;
		else
			mpdlist->scroll_top += (mpdlist->timed_scroll
				- mpdlist->scroll_top) * 0.5;
	}
}

static int timed_draw(void *data)
{
	mtk_mpdlist_t *mpdlist = data;

	scroll_fixup(mpdlist);
	draw((mtk_widget_t*)data);

	if (mpdlist->scroll_top == mpdlist->timed_scroll) {
		mpdlist->timed_active = 0;
		return 0;
	}
	else
		return 1;
}

static void mouse_press(mtk_widget_t *widget, int x, int y)
{
	mtk_mpdlist_t *mpdlist = (mtk_mpdlist_t*)widget;

	if (y <= UNIT) {
		mpdlist->slide_scroll = 0;
		mpdlist->timed_scroll -= (widget->h/2 - (widget->h/2)%UNIT);
		if (!mpdlist->timed_active) {
			mpdlist->timed_active = 1;
			mtk_timer_add(0.08, timed_draw, mpdlist);
		}
	}
	else if (y > widget->h-UNIT) {
		mpdlist->slide_scroll = 0;
		mpdlist->timed_scroll += (widget->h/2 - (widget->h/2)%UNIT);
		if (!mpdlist->timed_active) {
			mpdlist->timed_active = 1;
			mtk_timer_add(0.08, timed_draw, mpdlist);
		}
	}
	else {
		mpdlist->slide_scroll = 1;
		mpdlist->timed_scroll = mpdlist->scroll_top;
		mpdlist->slide_start = mpdlist->scroll_top;
		mpdlist->slide_offset = y;
	}

	scroll_fixup(mpdlist);
	draw(widget);
}

static void mouse_release(mtk_widget_t *widget, int x, int y)
{
	mtk_mpdlist_t *mpdlist = (mtk_mpdlist_t*)widget;

	if (!mpdlist->slide_scroll)
		return;

	if (!mpdlist->slide_scroll_moved && mpdlist->clicked) {
		int pos = (float)(y - UNIT) / UNIT +
			(float)mpdlist->scroll_top / UNIT;

		if (pos < mtk_list_length(mpdlist->list)) {
			if (mpdlist->clicked(&mpdlist->data,
			    mpdlist->list, pos)) {
				mpdlist->timed_scroll = 0;
				mpdlist->scroll_top = 0;
				update(widget);
			}
		}
	}
	else if (mpdlist->slide_scroll_moved) {
		/* slide scroll done, adjust to nearest item */
		mpdlist->timed_scroll = mpdlist->slide_start - y +
			mpdlist->slide_offset;

		if (mpdlist->slide_offset < y)
			mpdlist->timed_scroll -=
				abs(mpdlist->timed_scroll) % UNIT;
		else
			mpdlist->timed_scroll +=
				UNIT - abs(mpdlist->timed_scroll) % UNIT;

		if (!mpdlist->timed_active) {
			mpdlist->timed_active = 1;
			mtk_timer_add(0.08, timed_draw, mpdlist);
		}
	}

	mpdlist->slide_scroll = 0;
	mpdlist->slide_scroll_moved = 0;

	scroll_fixup(mpdlist);
	draw(widget);
}

static void mouse_move(mtk_widget_t *widget, int x, int y)
{
	mtk_mpdlist_t *mpdlist = (mtk_mpdlist_t*)widget;

	if (!mpdlist->slide_scroll)
		return;

	if (!mpdlist->slide_scroll_moved && abs(y - mpdlist->slide_offset) < 5)
		return;

	mpdlist->scroll_top = mpdlist->slide_start - y + mpdlist->slide_offset;
	mpdlist->timed_scroll = mpdlist->scroll_top;
	mpdlist->slide_scroll_moved = 1;

	scroll_fixup(mpdlist);
	draw(widget);
}

mtk_mpdlist_t* mtk_mpdlist_new(size_t size, int x, int y, int w, int h,
		void (*updatelist)(mtk_list_t *list, void *data),
		int (*clicked)(void **data, mtk_list_t *list, int pos),
		void *data)
{
	mtk_mpdlist_t *mpdlist = mtk_mpdlist(mtk_widget_new(size, x, y, w, h));

	SET_CLASS(mpdlist, mtk_mpdlist);

	mpdlist->scroll_surface =
		cairo_image_surface_create(CAIRO_FORMAT_RGB24, WIDTH, 4000);
	mpdlist->list = mtk_list_new();
	mpdlist->data = data;
	mpdlist->updatelist = updatelist;
	mpdlist->clicked = clicked;

	return mpdlist;
}

METHOD_TABLE_INIT(mtk_mpdlist, mtk_widget)
	METHOD(update);
	METHOD(draw);
	METHOD(mouse_press);
	METHOD(mouse_release);
	METHOD(mouse_move);
METHOD_TABLE_END
