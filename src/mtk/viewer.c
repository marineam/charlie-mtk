#include <assert.h>
#include <cairo.h>
#include <mtk.h>

#include "private.h"

/* for checking a widget is at a location */
#define IF_AT_XY(w,x,y) \
	if (x >= w->x && y >= w->y && \
	    x <= w->x + w->w && y <= w->y + w->h)

static void draw(mtk_widget_t *widget)
{
	mtk_widget_t *child = mtk_viewer(widget)->current;
	cairo_t *cr;

	if (!mtk_container(widget)->ran_init) {
		/* the container's draw method is called during init, but we
		 * must wait till init is over */
		return;
	}

	if (child) {
		cr = cairo_create(widget->surface);
		cairo_rectangle(cr, child->x, child->y, child->w, child->h);
		cairo_clip(cr);
		cairo_set_source_surface(cr, child->surface, child->x, child->y);
		cairo_paint(cr);

		cairo_destroy(cr);
	}

	super(widget,mtk_container,mtk_widget,draw);
}

static void add_widget(mtk_container_t *c, mtk_widget_t *w)
{
	mtk_viewer_t *v = mtk_viewer(c);

	if (!v->current)
		v->current = w;

	super(c,mtk_viewer,mtk_container,add_widget,w);
}

static void mouse_press(mtk_widget_t *c, int x, int y)
{
	mtk_widget_t *w = mtk_viewer(c)->current;

	IF_AT_XY(w, x, y)
		if (call_defined(w,mtk_widget,mouse_press))
			call(w,mtk_widget,mouse_press, x-w->x, y-w->y);
}

static void mouse_release(mtk_widget_t *c, int x, int y)
{
	mtk_widget_t *w = mtk_viewer(c)->current;

	IF_AT_XY(w, x, y)
		if (call_defined(w,mtk_widget,mouse_release))
			call(w,mtk_widget,mouse_release, x-w->x, y-w->y);
}

static void mouse_move(mtk_widget_t *c, int x, int y)
{
	mtk_widget_t *w = mtk_viewer(c)->current;

	IF_AT_XY(w, x, y)
		if (call_defined(w,mtk_widget,mouse_move))
			call(w,mtk_widget,mouse_move, x-w->x, y-w->y);
}

mtk_viewer_t* mtk_viewer_new(size_t size)
{
	mtk_viewer_t *this = mtk_viewer(mtk_container_new(size));
	SET_CLASS(this, mtk_viewer);
	return this;
}

METHOD_TABLE_INIT(mtk_viewer, mtk_container)
	METHOD(draw);
	METHOD(mouse_press);
	METHOD(mouse_release);
	METHOD(mouse_move);
	METHOD(add_widget);
METHOD_TABLE_END
