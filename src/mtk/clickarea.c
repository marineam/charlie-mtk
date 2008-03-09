#include <cairo.h>
#include <mtk.h>

#include "private.h"

struct clickarea {
	mtk_widget_t widget;
	int line;
};

static void draw(mtk_widget_t *widget)
{
	struct clickarea *area = (struct clickarea*)widget;
	cairo_t *cr;

	cr = cairo_create(widget->window->surface);

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, widget->x, widget->y, widget->w, widget->h);
	cairo_fill(cr);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, 2);
	cairo_move_to(cr, widget->x, area->line);
	cairo_line_to(cr, widget->x+widget->w, area->line);
	cairo_stroke(cr);

	cairo_destroy(cr);
}

static void click(mtk_widget_t *widget, int x, int y)
{
	struct clickarea *area = (struct clickarea*)widget;

	area->line = y;
	draw(widget);
}

mtk_widget_t* mtk_clickarea_new(int x, int y, int w, int h)
{
	struct clickarea *area = xmalloc0(sizeof(struct clickarea));

	area->widget.x = x;
	area->widget.y = y;
	area->widget.w = w;
	area->widget.h = h;
	area->widget.draw = draw;
	area->widget.mouse_press = click;
	area->widget.mouse_move = click;
	area->line = h/2;

	return (mtk_widget_t*)area;
}
