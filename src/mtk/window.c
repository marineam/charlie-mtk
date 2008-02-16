#include <cairo.h>
#include <gtk/gtk.h>
#include "window.h"

static gboolean redraw(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	cairo_t *cr;
	gint w, h;

	gtk_window_get_size(GTK_WINDOW(widget), &w, &h);

	cr = gdk_cairo_create(widget->window);

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 0, 0, w, h);
	cairo_fill(cr);

	cairo_destroy(cr);

	return FALSE;
}

GtkWidget* mtk_window_new(gint w, gint h)
{
	GtkWidget *window;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

	//g_signal_connect(window, "expose-event",
	//		G_CALLBACK (redraw), NULL);
	g_signal_connect(window, "destroy",
			G_CALLBACK (gtk_main_quit), NULL);

	gtk_window_set_default_size(GTK_WINDOW(window), w, h);
	gtk_widget_set_app_paintable(window, TRUE);

	return window;
}
