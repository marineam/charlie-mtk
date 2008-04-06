#include <string.h>
#include <assert.h>
#include <cairo.h>
#include <mtk.h>
#include <libmpdclient.h>

#include "private.h"

void _mtk_widget_new(mtk_widget_t *widget, int x, int y, int w, int h)
{
	widget->x = x;
	widget->y = y;
	widget->w = w;
	widget->h = h;
}
