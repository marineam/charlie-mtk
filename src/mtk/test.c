#include <gtk/gtk.h>
#include <mtk.h>

int main (int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *box;

	gtk_init(&argc, &argv);

	window = mtk_window_new(640, 480);
	box = gtk_hbox_new(TRUE, 0);
	gtk_container_add(GTK_CONTAINER(window), box);
	gtk_box_pack_start(GTK_BOX(box), mtk_clickarea_new(), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box), mtk_clickarea_new(), TRUE, TRUE, 0);

	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}
