#include <string.h>
#include <assert.h>
#include <cairo.h>
#include <mtk.h>
#include <libmpdclient.h>

#include "private.h"

struct mpdlist {
	mtk_widget_t widget;
	mtk_list_t* list;
	int timed_scroll;
	int slide_scroll;
	int slide_start;
	int slide_offset;
	int slide_scroll_moved;
	int scroll_top;
	void (*updatelist)(mtk_list_t *list, void *data);
	int (*clicked)(void **data, mtk_list_t *list, int pos);
	void *data;
};

static void update(mtk_widget_t *widget)
{
	struct mpdlist *mpdlist = (struct mpdlist*)widget;
	cairo_t *cr = cairo_create(widget->surface);
	mpd_InfoEntity *entity;
	int y = 0;

	if (mpdlist->updatelist)
		mpdlist->updatelist(mpdlist->list, mpdlist->data);

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 0, 0, WIDTH, 4000);
	cairo_fill(cr);

	cairo_set_line_width(cr, 2);
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_select_font_face(cr, "Sans",
		CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 20.0);

	mtk_list_foreach(mpdlist->list, entity) {
		cairo_move_to(cr, 0, y-1);
		cairo_line_to(cr, WIDTH, y-1);
		cairo_stroke(cr);
		cairo_move_to(cr, 0, y+30);
		if (entity->type == MPD_INFO_ENTITY_TYPE_DIRECTORY) {
			/* note: using the gnu version of basename */
			char *path = strdup(basename(
				entity->info.directory->path));
			for (int i = 0; i < strlen(path); i++) {
				if (path[i] == '_')
					path[i] = ' ';
			}
			cairo_show_text(cr, path);
			free(path);
		}
		y += 40;
	}

	cairo_destroy(cr);
}

static void draw(mtk_widget_t *widget)
{
	struct mpdlist *mpdlist = (struct mpdlist*)widget;
	cairo_t *cr;

	cr = cairo_create(widget->window->surface);

	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_rectangle(cr, 0, 0, widget->w, UNIT-2);
	cairo_rectangle(cr, 0, widget->h - UNIT, widget->w, widget->h);
	cairo_fill(cr);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_move_to(cr, 0, UNIT-1);
	cairo_line_to(cr, widget->w, UNIT-1);
	cairo_move_to(cr, 0, widget->h - UNIT-1);
	cairo_line_to(cr, widget->w, widget->h - UNIT-1);
	cairo_stroke(cr);

	cairo_rectangle(cr, 0, UNIT, widget->w, widget->h - (2*UNIT)-2);
	cairo_clip(cr);
	cairo_set_source_surface(cr, widget->surface, 0,
		-mpdlist->scroll_top + UNIT);
	cairo_paint(cr);

	cairo_destroy(cr);
}

/* cleans things up after mouse events */
static void scroll_fixup(struct mpdlist *mpdlist)
{
	/* extreme scroll positions, max is the top, min bottom */
	int min = 0;
	int max = (mtk_list_length(mpdlist->list) * UNIT) -
		(mpdlist->widget.h - 2*UNIT);

	if (mpdlist->timed_scroll > max)
		mpdlist->timed_scroll = max;
	else if (mpdlist->timed_scroll < min)
		mpdlist->timed_scroll = min;

	if (mpdlist->scroll_top > max)
		mpdlist->scroll_top = max;
	else if (mpdlist->scroll_top < min)
		mpdlist->scroll_top = min;

	if (mpdlist->timed_scroll != mpdlist->scroll_top) {
		if (mpdlist->timed_scroll+1 == mpdlist->scroll_top ||
		    mpdlist->timed_scroll-1 == mpdlist->scroll_top)
			mpdlist->scroll_top = mpdlist->timed_scroll;
		else
			mpdlist->scroll_top += (mpdlist->timed_scroll
				- mpdlist->scroll_top) * 0.5;
	}
}

static int timed_draw(void *data)
{
	scroll_fixup((struct mpdlist*)data);
	draw((mtk_widget_t*)data);
	return 1;
}

static void mouse_press(mtk_widget_t *widget, int x, int y)
{
	struct mpdlist *mpdlist = (struct mpdlist*)widget;

	if (y <= UNIT) {
		mpdlist->slide_scroll = 0;
		mpdlist->timed_scroll -= (widget->h/2 - (widget->h/2)%UNIT);
	}
	else if (y > widget->h-UNIT) {
		mpdlist->slide_scroll = 0;
		mpdlist->timed_scroll += (widget->h/2 - (widget->h/2)%UNIT);
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
	struct mpdlist *mpdlist = (struct mpdlist*)widget;

	if (!mpdlist->slide_scroll)
		return;

	if (!mpdlist->slide_scroll_moved && mpdlist->clicked) {
		int pos = (float)(y - UNIT) / UNIT +
			(float)mpdlist->scroll_top / UNIT;

		if (mpdlist->clicked(&mpdlist->data, mpdlist->list, pos)) {
			mpdlist->timed_scroll = 0;
			mpdlist->scroll_top = 0;
		}
		update(widget);
		return;
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
	}

	mpdlist->slide_scroll = 0;
	mpdlist->slide_scroll_moved = 0;

	scroll_fixup(mpdlist);
	draw(widget);
}

static void mouse_move(mtk_widget_t *widget, int x, int y)
{
	struct mpdlist *mpdlist = (struct mpdlist*)widget;

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

mtk_widget_t* mtk_mpdlist_new(int x, int y, int w, int h,
	void (*updatelist)(mtk_list_t *list, void *data),
	int (*clicked)(void **data, mtk_list_t *list, int pos),
	void *data)
{
	struct mpdlist *mpdlist = xmalloc0(sizeof(struct mpdlist));

	mpdlist->widget.x = x;
	mpdlist->widget.y = y;
	mpdlist->widget.w = w;
	mpdlist->widget.h = h;
	mpdlist->widget.surface =
		cairo_image_surface_create(CAIRO_FORMAT_RGB24, WIDTH, 4000);
	mpdlist->widget.update = update;
	mpdlist->widget.draw = draw;
	mpdlist->widget.mouse_press = mouse_press;
	mpdlist->widget.mouse_release = mouse_release;
	mpdlist->widget.mouse_move = mouse_move;
	mpdlist->list = mtk_list_new();
	mpdlist->data = data;
	mpdlist->updatelist = updatelist;
	mpdlist->clicked = clicked;

	update(&mpdlist->widget);

	//mtk_timer_add(0.08, timed_draw, mpdlist);

	return (mtk_widget_t*)mpdlist;
}
