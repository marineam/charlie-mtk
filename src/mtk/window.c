#include <cairo.h>
#include <gtk/gtk.h>
#include "window.h"

GtkWidget* mtk_window_new(gint w, gint h)
{
	GtkWidget *window;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

	g_signal_connect(window, "destroy",
			G_CALLBACK (gtk_main_quit), NULL);

	gtk_window_set_default_size(GTK_WINDOW(window), w, h);

	return window;
}
