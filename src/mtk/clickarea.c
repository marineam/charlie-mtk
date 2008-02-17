#include <cairo.h>
#include <gtk/gtk.h>
#include "clickarea.h"

static gboolean redraw(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	static double y = -999;
	cairo_t *cr;
	gint w, h;

	cr = gdk_cairo_create(widget->window);
	w = widget->allocation.width;
	h = widget->allocation.height;

	if (event->type == GDK_BUTTON_PRESS)
		y = event->button.y;
	if (event->type == GDK_MOTION_NOTIFY)
		y = event->motion.y;

	if (y == -999)
		y = h/2;

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 0, 0, w, h);
	cairo_fill(cr);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, 2);
	cairo_move_to(cr, 0, y);
	cairo_line_to(cr, w, y);
	cairo_stroke(cr);

	cairo_destroy(cr);

	return FALSE;
}


GtkWidget* mtk_clickarea_new()
{
	GtkWidget *area;

	area = gtk_drawing_area_new();
	gtk_widget_add_events(area, GDK_BUTTON_PRESS_MASK |
			GDK_BUTTON1_MOTION_MASK);

	g_signal_connect(area, "expose-event",
			G_CALLBACK (redraw), NULL);
	g_signal_connect(area, "button-press-event",
			G_CALLBACK (redraw), NULL);
	g_signal_connect(area, "motion-notify-event",
			G_CALLBACK (redraw), NULL);

	return area;
}
