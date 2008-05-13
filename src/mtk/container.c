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
		super(c,mtk_container,mtk_widget,init,parent);
	assert(c->surface);

	mtk_list_foreach(mtk_container(c)->widgets, w) {
		call(w,mtk_widget,init,c);
	}

	mtk_container(c)->ran_init = 1;
	call(c,mtk_widget,draw);
}

static void draw_widget(mtk_container_t* c, mtk_widget_t* widget) {
	cairo_t *cr = cairo_create(mtk_widget(c)->surface);

	cairo_rectangle(cr, widget->x, widget->y, widget->w, widget->h);
	cairo_clip(cr);
	cairo_set_source_surface(cr, widget->surface, widget->x, widget->y);
	cairo_paint(cr);

	cairo_destroy(cr);
}

static void add_widget(mtk_container_t* c, mtk_widget_t* widget)
{
	mtk_list_append(c->widgets, widget);
	call(widget,mtk_widget,set_parent, mtk_widget(c));

	/* if size is 0 auto-fit the widget */
	if (!widget->w || !widget->h)
		call(widget,mtk_widget,set_size,
			mtk_widget(c)->w, mtk_widget(c)->h);

	if (c->ran_init) {
		/* only init child if the container's init has run */
		assert(call_defined(widget,mtk_widget,init));
		call(widget,mtk_widget,init, mtk_widget(c));
	}
}

static void draw(mtk_widget_t *c)
{
	mtk_widget_t *w;

	if (!mtk_container(c)->ran_init) {
		/* the container's draw method is called during init, but we
		 * must wait till init is over */
		return;
	}

	mtk_list_foreach(mtk_container(c)->widgets, w)
		draw_widget(mtk_container(c), w);

	super(c,mtk_container,mtk_widget,draw);
}

static void mouse_press(mtk_widget_t *c, int x, int y)
{
	mtk_widget_t *w;

	FOREACH_AT_XY(mtk_container(c)->widgets, w, x, y)
		if (call_defined(w,mtk_widget,mouse_press))
			call(w,mtk_widget,mouse_press, x-w->x, y-w->y);
}

static void mouse_release(mtk_widget_t *c, int x, int y)
{
	mtk_widget_t *w;

	FOREACH_AT_XY(mtk_container(c)->widgets, w, x, y)
		if (call_defined(w,mtk_widget,mouse_release))
			call(w,mtk_widget,mouse_release, x-w->x, y-w->y);
}

static void mouse_move(mtk_widget_t *c, int x, int y)
{
	mtk_widget_t *w;

	FOREACH_AT_XY(mtk_container(c)->widgets, w, x, y)
		if (call_defined(w,mtk_widget,mouse_move))
			call(w,mtk_widget,mouse_move, x-w->x, y-w->y);
}

mtk_container_t* mtk_container_new(size_t size)
{
	mtk_container_t *c = mtk_container(mtk_widget_new(size));
	SET_CLASS(c, mtk_container);
	c->widgets = mtk_list_new();

	return c;
}

METHOD_TABLE_INIT(mtk_container, mtk_widget)
	METHOD(init);
	METHOD(draw);
	METHOD(mouse_press);
	METHOD(mouse_release);
	METHOD(mouse_move);
	METHOD(add_widget);
METHOD_TABLE_END
