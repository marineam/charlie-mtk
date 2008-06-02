#include <assert.h>
#include <cairo.h>
#include <mtk.h>

#include "private.h"

/* for finding widgets at a location */
#define FOREACH_AT_XY(l,w,x,y) \
	mtk_list_foreach(l, w) \
		if (x >= w->x && y >= w->y && \
		    x <= w->x + w->w && y <= w->y + w->h)

/* This overrides the normal widget init method to run
 * init on all child widgets as well as itself */
static void init(void *this, mtk_widget_t* parent)
{
	mtk_container_t *c = this;
	mtk_widget_t *w;

	if (parent)
		super(c,mtk_container,init,parent);

	assert(mtk_widget(c)->surface);

	mtk_list_foreach(c->widgets, w) {
		call(w,init, mtk_widget(c));
	}

	c->ran_init = true;
}

static void draw_widget(void *this, mtk_widget_t* widget)
{
	mtk_container_t *c = this;
	cairo_t *cr;

	if (widget->redraw)
		call(widget,draw);

	cr = cairo_create(mtk_widget(c)->surface);

	cairo_rectangle(cr, widget->x, widget->y, widget->w, widget->h);
	cairo_clip(cr);
	cairo_set_source_surface(cr, widget->surface, widget->x, widget->y);
	cairo_paint(cr);

	cairo_destroy(cr);
}

static void add_widget(void *this, mtk_widget_t* widget)
{
	mtk_container_t *c = this;

	mtk_list_append(c->widgets, widget);
	call(widget,set_parent, mtk_widget(c));

	/* if size is 0 auto-fit the widget */
	if (!widget->w || !widget->h)
		call(widget,set_size, mtk_widget(c)->w, mtk_widget(c)->h);

	if (c->ran_init) {
		/* only init child if the container's init has run */
		assert(call_defined(widget,init));
		call(widget,init, mtk_widget(c));
	}
}

static void reorder_top(void *this, mtk_widget_t* widget)
{
	mtk_container_t *c = this;
	mtk_widget_t *w;

	w = mtk_list_goto(c->widgets, 0);
	while (w && w != widget)
		w = mtk_list_next(c->widgets);

	assert(w && w == widget);

	if (mtk_list_index(c->widgets) != 0) {
		mtk_list_remove(c->widgets);
		mtk_list_prepend(c->widgets, w);
	}
}

static void draw(void *this)
{
	mtk_container_t *c = this;
	mtk_widget_t *w;

	mtk_list_foreach_rev(c->widgets, w)
		draw_widget(c, w);

	super(c,mtk_container,draw);
}

static void mouse_press(void *this, int x, int y)
{
	mtk_widget_t *c = this;
	mtk_widget_t *w;

	FOREACH_AT_XY(mtk_container(c)->widgets, w, x, y) {
		if (call_defined(w,mouse_press))
			call(w,mouse_press, x-w->x, y-w->y);
		if (!mtk_container(c)->event_stacking)
			break;
	}
}

static void mouse_release(void *this, int x, int y)
{
	mtk_widget_t *c = this;
	mtk_widget_t *w;

	FOREACH_AT_XY(mtk_container(c)->widgets, w, x, y) {
		if (call_defined(w,mouse_release))
			call(w,mouse_release, x-w->x, y-w->y);
		if (!mtk_container(c)->event_stacking)
			break;
	}
}

static void mouse_move(void *this, int x, int y)
{
	mtk_widget_t *c = this;
	mtk_widget_t *w;

	FOREACH_AT_XY(mtk_container(c)->widgets, w, x, y) {
		if (call_defined(w,mouse_move))
			call(w,mouse_move, x-w->x, y-w->y);
		if (!mtk_container(c)->event_stacking)
			break;
	}
}

static void set_size(void *vthis, int w, int h)
{
	mtk_widget_t *this = vthis;
	mtk_widget_t *widget;

	super(this,mtk_container,set_size, w, h);

	mtk_list_foreach(mtk_container(this)->widgets, widget)
		call(widget,set_size, w, h);
}

static void objfree(void *vthis)
{
	mtk_container_t *this = vthis;

	mtk_list_free_obj(this->widgets);

	super(this,mtk_container,free);
}

mtk_container_t* mtk_container_new(size_t size)
{
	mtk_container_t *c = mtk_container(mtk_widget_new(size));
	SET_CLASS(c, mtk_container);
	c->widgets = mtk_list_new();

	return c;
}

METHOD_TABLE_INIT(mtk_container, mtk_widget)
	_METHOD(free, objfree);
	METHOD(init);
	METHOD(draw);
	METHOD(mouse_press);
	METHOD(mouse_release);
	METHOD(mouse_move);
	METHOD(set_size);
	METHOD(add_widget);
	METHOD(reorder_top);
METHOD_TABLE_END
