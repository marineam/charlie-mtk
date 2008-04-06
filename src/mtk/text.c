#include <string.h>
#include <assert.h>
#include <cairo.h>
#include <mtk.h>
#include <libmpdclient.h>

#include "private.h"

struct text {
	mtk_widget_t widget;
	char *text;
	double size;
};

static void draw(mtk_widget_t *widget)
{
	struct text *text = (struct text*)widget;
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
}

mtk_widget_t* mtk_text_new(int x, int y, int w, int h, char *t)
{
	struct text *text = xmalloc0(sizeof(struct text));

	text->widget.x = x;
	text->widget.y = y;
	text->widget.w = w;
	text->widget.h = h;
	text->widget.draw = draw;
	text->text = t;

	return (mtk_widget_t*)text;
}
