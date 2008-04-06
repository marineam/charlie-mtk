#include <assert.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <cairo.h>
#include <cairo-xcb.h>
#include <mtk.h>

#include "private.h"

/* for finding widgets at a location */
#define FOREACH_AT_XY(l,w,x,y) \
	mtk_list_foreach(l, w) \
		if (x >= w->x && y >= w->y && \
		    x <= w->x + w->w && y <= w->y + w->h)

static void draw_widget(mtk_container_t* c, mtk_widget_t* widget) {
	cairo_t *cr = cairo_create(MTK_WIDGET(c)->surface);

	cairo_rectangle(cr, widget->x, widget->y, widget->w, widget->h);
	cairo_clip(cr);
	cairo_set_source_surface(cr, widget->surface, widget->x, widget->y);
	cairo_paint(cr);

	cairo_destroy(cr);
}

void mtk_container_add(mtk_container_t* c, mtk_widget_t* widget)
{
	mtk_list_append(c->widgets, widget);

	assert(MTK_WIDGET(c)->surface);
	widget->surface = cairo_surface_create_similar(
			MTK_WIDGET(c)->surface,
			CAIRO_CONTENT_COLOR,
			MTK_WIDGET(c)->w, MTK_WIDGET(c)->h);

	if (widget->update)
		widget->update(widget);
	assert(widget->draw);
	widget->draw(widget);
	draw_widget(c, widget);
}

static void draw(mtk_widget_t *c)
{
	mtk_widget_t *w;

	mtk_list_foreach(MTK_CONTAINER(c)->widgets, w)
		draw_widget(MTK_CONTAINER(c), w);
}

static void mouse_press(mtk_widget_t *c, int x, int y)
{
	mtk_widget_t *w;

	FOREACH_AT_XY(MTK_CONTAINER(c)->widgets, w, x, y)
		if (w->mouse_press)
			w->mouse_press(w, x-w->x, y-w->y);
}

static void mouse_release(mtk_widget_t *c, int x, int y)
{
	mtk_widget_t *w;

	FOREACH_AT_XY(MTK_CONTAINER(c)->widgets, w, x, y)
		if (w->mouse_release)
			w->mouse_release(w, x-w->x, y-w->y);
}

static void mouse_move(mtk_widget_t *c, int x, int y)
{
	mtk_widget_t *w;

	FOREACH_AT_XY(MTK_CONTAINER(c)->widgets, w, x, y)
		if (w->mouse_move)
			w->mouse_move(w, x-w->x, y-w->y);
}

void _mtk_container_new(mtk_container_t *c, int x, int y, int w, int h)
{
	_mtk_widget_new(MTK_WIDGET(c), x, y, w, h);
	MTK_WIDGET(c)->draw = draw;
	MTK_WIDGET(c)->mouse_press = mouse_press;
	MTK_WIDGET(c)->mouse_release = mouse_release;
	MTK_WIDGET(c)->mouse_move = mouse_move;
	c->widgets = mtk_list_new();
}

mtk_container_t* mtk_container_new(int x, int y, int w, int h)
{
	mtk_container_t *c = xmalloc0(sizeof(mtk_container_t));
	_mtk_container_new(c, x, y, w, h);
	return c;
}

