#include <string.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include <libmpdclient.h>
#include <charlie.h>
#include <mtk.h>
#include <config.h>

static void update(cairo_surface_t *surface, const char *dir)
{
	int y = 0;
	cairo_t *cr = cairo_create(surface);
	GList *list = mpd_get_dir(dir);

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 0, 0, WIDTH, 4000);
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
		cairo_line_to(cr, WIDTH, y-1);
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
}

static gboolean redraw(GtkWidget *widget, GdkEvent *event, cairo_surface_t* sr)
{
	static int top = 0, start = 0, offset = 0;
	//int w, h;
	cairo_t *cr;

	//w = widget->allocation.width;
	//h = widget->allocation.height;
	cr = gdk_cairo_create(widget->window);

	if (event->type == GDK_BUTTON_PRESS) {
		start = top;
		offset = event->button.y;
	}
	else if (event->type == GDK_MOTION_NOTIFY)
		top = start + event->motion.y - offset;

	cairo_set_source_surface(cr, sr, 0, top);
	cairo_paint(cr);
	cairo_destroy(cr);

	return FALSE;
}


GtkWidget* mtk_mpdlist_new()
{
	GtkWidget *area;
	cairo_surface_t *surface;

	surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, WIDTH, 4000);
	update(surface, "");

	area = gtk_drawing_area_new();
	gtk_widget_add_events(area, GDK_BUTTON_PRESS_MASK |
			GDK_BUTTON1_MOTION_MASK);

	g_signal_connect(area, "expose-event",
			G_CALLBACK (redraw), surface);
	g_signal_connect(area, "button-press-event",
			G_CALLBACK (redraw), surface);
	g_signal_connect(area, "motion-notify-event",
			G_CALLBACK (redraw), surface);

	return area;
}
