#include <string.h>
#include <assert.h>
#include <cairo.h>
#include <mtk.h>
#include <libmpdclient.h>

#include "private.h"

static bool scroller(void *data)
{
	mtk_text_t *this = data;

	if (this->scroll_stop)
		return 0;

	this->scroll -= 1;
	call(this,mtk_widget,draw);

	return this->scroll ? true : false;
}

static bool start_scroll(void *data)
{
	mtk_text_t *this = data;

	this->scroll_stop = false;
	mtk_timer_add(1.0/30, scroller, this);

	return false;
}


static void draw(mtk_widget_t *widget)
{
	mtk_text_t *text = mtk_text(widget);
	cairo_t *cr = cairo_create(widget->surface);
	cairo_font_extents_t fe;
	cairo_text_extents_t te;

	assert(text->text);

	cairo_select_font_face(cr, "Sans",
		CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, widget->h);
	cairo_font_extents(cr, &fe);
	/* adjust font size so h == fe.height */
	cairo_set_font_size(cr, widget->h * (widget->h/fe.height));
	cairo_font_extents(cr, &fe);
	cairo_text_extents(cr, text->text, &te);

	if (text->scroll + te.width + widget->h*2 < 0) {
		text->scroll = 0;
		text->scroll_stop = true;
	}

	if (!text->scroll && te.width > widget->w)
		mtk_timer_add(2.0, start_scroll, widget);

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 0, 0, widget->w, widget->h);
	cairo_fill(cr);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_move_to(cr, text->scroll, fe.ascent);
	cairo_show_text(cr, text->text);

	if (te.width > widget->w &&
	    text->scroll + te.width + widget->h*2 < widget->w) {
		cairo_move_to(cr, text->scroll+te.width+widget->h*2, fe.ascent);
		cairo_show_text(cr, text->text);
	}

	cairo_destroy(cr);

	super(widget,mtk_text,mtk_widget,draw);
}

static void set_text(mtk_text_t *this, char *text)
{
	assert(text);
	if (strcmp(this->text, text)) {
		free(this->text);
		this->text = strdup(text);
		assert(this->text);
		this->scroll = 0;
		this->scroll_stop = true;
		call(this,mtk_widget,draw);
	}
}

mtk_text_t* mtk_text_new(size_t size, char *text)
{
	mtk_text_t *this = mtk_text(mtk_widget_new(size));
	SET_CLASS(this, mtk_text);
	assert(text);
	this->text = strdup(text);
	assert(this->text);
	return this;
}

METHOD_TABLE_INIT(mtk_text, mtk_widget)
	METHOD(set_text);
	METHOD(draw);
METHOD_TABLE_END
