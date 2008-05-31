#include <time.h>
#include <config.h>
#include <charlie.h>
#include <mtk.h>

int main (int argc, char *argv[])
{
	mtk_window_t *window;
	mtk_menu_t *view;

	mtk_init();
	_mpd_dirlist_class_init();
	_mpd_playlist_class_init();
	_mpd_status_class_init();

	window = new(mtk_window, WIDTH, HEIGHT);
	view = new(mtk_menu);
	call(window,add_widget, mtk_widget(view));
	mpd_init(view);

	mtk_main();

	mtk_cleanup();

	return 0;
}
