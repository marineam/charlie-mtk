#include <string.h>
#include <assert.h>
#include <cairo.h>
#include <mtk.h>

#include "private.h"

static void draw(void *this)
{
	mtk_widget_t *widget = this;
	mtk_image_t *image = this;
	cairo_t *cr = cairo_create(widget->surface);
	int w, h, x = 0, y = 0;
	double scale;

	w = cairo_image_surface_get_width(image->image);
	h = cairo_image_surface_get_height(image->image);

	if (w/h) {
		scale = (double)widget->w/w;
		y = ((1.0/scale)*widget->h-h)/2;
	}
	else {
		scale = (double)widget->h/h;
		x = ((1.0/scale)*widget->w-w)/2;
	}

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 0, 0, widget->w, widget->h);
	cairo_fill(cr);

	cairo_scale(cr, scale, scale);
	cairo_set_source_surface(cr, image->image, x, y);
	cairo_paint(cr);

	cairo_destroy(cr);

	super(widget,mtk_image,draw);
}

static void set_image(void *vthis, char *path)
{
	mtk_image_t *this = vthis;

	assert(path);
	if (strcmp(this->path, path)) {
		free(this->path);
		this->path = strdup(path);
		cairo_surface_destroy(this->image);
		this->image = cairo_image_surface_create_from_png(path);
		call(this,redraw);
	}
}

mtk_image_t* mtk_image_new(size_t size, char *path)
{
	mtk_image_t *this = mtk_image(mtk_widget_new(size));
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
