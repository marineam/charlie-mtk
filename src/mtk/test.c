#include <cairo.h>
#include <gtk/gtk.h>
#include <stdio.h>

static gboolean redraw(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	static double y = -1;
	cairo_t *cr;
	gint w, h;

	gtk_window_get_size(GTK_WINDOW(widget), &w, &h);

	if (event->type == GDK_BUTTON_PRESS) {
		/* we assume the gdk window size == the gtk window size */
		y = event->button.y;
	}

	if (y < 0)
		y = h/2;

	cr = gdk_cairo_create(widget->window);

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 0, 0, w, h);
	cairo_fill(cr);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, 2);
	cairo_move_to(cr, 0, y);
	cairo_line_to(cr, w, y);
	cairo_stroke(cr);

	cairo_destroy(cr);

	printf("redraw!\n");

	return FALSE;
}


int
main (int argc, char *argv[])
{
	GtkWidget *window;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

	g_signal_connect(window, "expose-event",
			G_CALLBACK (redraw), NULL);
	g_signal_connect(window, "button-press-event",
			G_CALLBACK (redraw), NULL);
	g_signal_connect(window, "destroy",
			G_CALLBACK (gtk_main_quit), NULL);

	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
	gtk_widget_set_app_paintable(window, TRUE);

	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}
