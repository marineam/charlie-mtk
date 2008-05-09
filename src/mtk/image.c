#include <string.h>
#include <assert.h>
#include <cairo.h>
#include <mtk.h>

#include "private.h"

static void draw(mtk_widget_t *widget)
{
	mtk_image_t *image = mtk_image(widget);
	cairo_t *cr = cairo_create(widget->surface);
	int w, h;
	double scale;

	w = cairo_image_surface_get_width(image->image);
	h = cairo_image_surface_get_height(image->image);

	if (w/h)
		scale = (double)widget->w/w;
	else
		scale = (double)widget->h/h;

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 0, 0, widget->w, widget->h);
	cairo_fill(cr);

	cairo_scale(cr, scale, scale);
	cairo_set_source_surface(cr, image->image, 0, 0);
	cairo_paint(cr);

	cairo_destroy(cr);

	super(widget,mtk_image,mtk_widget,draw);
}

static void set_image(mtk_image_t *this, char *path)
{
	assert(path);
	if (strcmp(this->path, path)) {
		free(this->path);
		this->path = strdup(path);
		cairo_surface_destroy(this->image);
		this->image = cairo_image_surface_create_from_png(path);
		call(this,mtk_widget,draw);
	}
}

mtk_image_t* mtk_image_new(size_t size, int x, int y, int w, int h, char *path)
{
	mtk_image_t *this = mtk_image(mtk_widget_new(size, x, y, w, h));
	SET_CLASS(this, mtk_image);
	assert(path);
	this->path = strdup(path);
	this->image = cairo_image_surface_create_from_png(path);
	assert(this->path);
	assert(!cairo_surface_status(this->image));
	return this;
}

METHOD_TABLE_INIT(mtk_image, mtk_widget)
	METHOD(set_image);
	METHOD(draw);
METHOD_TABLE_END
