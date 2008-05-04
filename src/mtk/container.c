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

/* This overrides the normal widget init method to run
 * init on all child widgets as well as itself */
static void init(mtk_widget_t* c, mtk_widget_t* parent)
{
	mtk_widget_t *w;

	if (parent)
		super(c,mtk_widget,init,parent);
	assert(c->surface);

	mtk_list_foreach(MTK_CONTAINER(c)->widgets, w) {
		call(w,mtk_widget,init,c);
	}

	MTK_CONTAINER(c)->ran_init = 1;
	call(c,mtk_widget,draw);
}

static void draw_widget(mtk_container_t* c, mtk_widget_t* widget) {
	cairo_t *cr = cairo_create(MTK_WIDGET(c)->surface);

	cairo_rectangle(cr, widget->x, widget->y, widget->w, widget->h);
	cairo_clip(cr);
	cairo_set_source_surface(cr, widget->surface, widget->x, widget->y);
	cairo_paint(cr);

	cairo_destroy(cr);
}

static void add_widget(mtk_container_t* c, mtk_widget_t* widget)
{
	mtk_list_append(c->widgets, widget);

	if (c->ran_init) {
		/* only init child if the container's init has run */
		assert(call_defined(widget,mtk_widget,init));
		call(widget,mtk_widget,init, MTK_WIDGET(c));
	}

	draw_widget(c, widget);
}

static void draw(mtk_widget_t *c)
{
	mtk_widget_t *w;

	if (!MTK_CONTAINER(c)->ran_init) {
		/* the container's draw method is called during init, but we
		 * must wait till init is over */
		return;
	}

	mtk_list_foreach(MTK_CONTAINER(c)->widgets, w)
		draw_widget(MTK_CONTAINER(c), w);
}

static void mouse_press(mtk_widget_t *c, int x, int y)
{
	mtk_widget_t *w;

	FOREACH_AT_XY(MTK_CONTAINER(c)->widgets, w, x, y)
		if (call_defined(w,mtk_widget,mouse_press))
			call(w,mtk_widget,mouse_press, x-w->x, y-w->y);
}

static void mouse_release(mtk_widget_t *c, int x, int y)
{
	mtk_widget_t *w;

	FOREACH_AT_XY(MTK_CONTAINER(c)->widgets, w, x, y)
		if (call_defined(w,mtk_widget,mouse_release))
			call(w,mtk_widget,mouse_release, x-w->x, y-w->y);
}

static void mouse_move(mtk_widget_t *c, int x, int y)
{
	mtk_widget_t *w;

	FOREACH_AT_XY(MTK_CONTAINER(c)->widgets, w, x, y)
		if (call_defined(w,mtk_widget,mouse_move))
			call(w,mtk_widget,mouse_move, x-w->x, y-w->y);
}

mtk_container_t* mtk_container_new(size_t size, int x, int y, int w, int h)
{
	mtk_container_t *c = MTK_CONTAINER(mtk_widget_new(size, x, y, w, h));
	SET_CLASS(c, mtk_container);
	c->widgets = mtk_list_new();

	return c;
}

METHOD_TABLE_INIT(mtk_container, mtk_widget)
	METHOD(mtk_widget, init)		= init;
	METHOD(mtk_widget, draw)		= draw;
	METHOD(mtk_widget, mouse_press)		= mouse_press;
	METHOD(mtk_widget, mouse_release)	= mouse_release;
	METHOD(mtk_widget, mouse_move)		= mouse_move;
	METHOD(mtk_container, add_widget)	= add_widget;
METHOD_TABLE_END