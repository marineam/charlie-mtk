#include <cairo.h>
#include <gtk/gtk.h>
#include <mtk.h>

static gboolean mouse(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
	/* this is where volume control will go... */
	//printf("drag: %lf\n", event->y);

	return FALSE;
}

GtkWidget* mtk_window_new(gint w, gint h)
{
	GtkWidget *window;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_add_events(window, GDK_BUTTON1_MOTION_MASK);

	g_signal_connect(window, "destroy",
			G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect(window, "motion-notify-event",
			G_CALLBACK (mouse), NULL);

	gtk_window_set_default_size(GTK_WINDOW(window), w, h);

	return window;
}
