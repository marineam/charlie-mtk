#include <string.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include <libmpdclient.h>
#include <charlie.h>
#include <mtk.h>

static gboolean redraw(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	static int top = 0, start = 0, offset = 0;
	int w, h, y;
	cairo_surface_t *buffer_surface;
	cairo_t *gcr, *cr;
	GList *list = mpd_get_dir("");

	w = widget->allocation.width;
	h = widget->allocation.height;
	buffer_surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, w, h);
	gcr = gdk_cairo_create(widget->window);
	cr = cairo_create(buffer_surface);

	if (event->type == GDK_BUTTON_PRESS) {
		start = top;
		offset = event->button.y;
	}
	else if (event->type == GDK_MOTION_NOTIFY)
		top = start + event->motion.y - offset;

	y = top;

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 0, 0, w, h);
	cairo_fill(cr);

	cairo_set_line_width(cr, 2);
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_select_font_face(cr, "Sans",
		CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 20.0);

	for (GList *item = list; item != NULL; item = item->next) {
		mpd_InfoEntity *entity = item->data;

		cairo_move_to(cr, 0, y-1);
		cairo_line_to(cr, w, y-1);
		cairo_stroke(cr);
		cairo_move_to(cr, 0, y+30);
		if (entity->type == MPD_INFO_ENTITY_TYPE_DIRECTORY) {
			/* note: using the gnu version of basename */
			char *path = strdup(basename(
				entity->info.directory->path));
			for (int i = 0; i < strlen(path); i++) {
				if (path[i] == '_')
					path[i] = ' ';
			}
			cairo_show_text(cr, path);
			free(path);
		}
		y += 40;
	}

	cairo_destroy(cr);

	cairo_set_source_surface(gcr, buffer_surface, 0, 0);
	cairo_paint(gcr);
	cairo_destroy(gcr);

	cairo_surface_destroy(buffer_surface);

	return FALSE;
}


GtkWidget* mtk_mpdlist_new()
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
