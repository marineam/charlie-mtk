#include <string.h>
#include <assert.h>
#include <cairo.h>
#include <mtk.h>
#include <libmpdclient.h>

#include "private.h"

static void draw(mtk_widget_t *widget)
{
	mtk_text_t *text = mtk_text(widget);
	cairo_t *cr = cairo_create(widget->surface);
	cairo_text_extents_t te;

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 0, 0, widget->w, widget->h);
	cairo_fill(cr);

	if (text->text) {
		cairo_set_source_rgb(cr, 0, 0, 0);
		cairo_select_font_face(cr, "Sans",
			CAIRO_FONT_SLANT_NORMAL,
			CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(cr, widget->h);

		cairo_text_extents(cr, text->text, &te);

		cairo_move_to(cr, 0, widget->h*0.5 + te.height*0.5);
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
		call(this,mtk_widget,draw);
	}
}

mtk_text_t* mtk_text_new(size_t size, int x, int y, int w, int h, char *text)
{
	mtk_text_t *this = mtk_text(mtk_widget_new(size, x, y, w, h));
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
