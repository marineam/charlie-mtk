#include <gtk/gtk.h>
#include <config.h>
#include <charlie.h>
#include <mtk.h>

int main (int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *hbox;

	gtk_init(&argc, &argv);
	mpd_init();

	window = mtk_window_new(WIDTH, HEIGHT);
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), hbox);
	gtk_box_pack_start_defaults(GTK_BOX(hbox), mtk_mpdlist_new());

	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}
