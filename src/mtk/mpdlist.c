#include <cairo.h>
#include <gtk/gtk.h>
#include <libmpdclient.h>
#include <charlie.h>
#include <mtk.h>

static gboolean redraw(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	int w, h, y = 0;
	cairo_t *cr;
	GList *list = mpd_get_dir("");

	cr = gdk_cairo_create(widget->window);
	w = widget->allocation.width;
	h = widget->allocation.height;

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 0, 0, w, h);
	cairo_fill(cr);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_select_font_face(cr, "Sans",
		CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 20.0);

	for (GList *item = list; item != NULL; item = item->next) {
		mpd_InfoEntity *entity = item->data;

		cairo_move_to(cr, 0, y);
		if (entity->type == MPD_INFO_ENTITY_TYPE_DIRECTORY)
			cairo_show_text(cr, entity->info.directory->path);
		y += 20;
	}

	cairo_destroy(cr);

	return FALSE;
}


GtkWidget* mtk_mpdlist_new()
{
	GtkWidget *area;

	area = gtk_drawing_area_new();
	gtk_widget_add_events(area, GDK_BUTTON_PRESS_MASK);

	g_signal_connect(area, "expose-event",
			G_CALLBACK (redraw), NULL);

	return area;
}
