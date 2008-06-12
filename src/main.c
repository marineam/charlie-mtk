#include <time.h>
#include <charlie.h>
#include <mtk.h>

int main (int argc, char *argv[])
{
	mtk_window_t *window;

	mtk_init();
	_mpd_main_class_init();
	_mpd_dirlist_class_init();
	_mpd_playlist_class_init();
	_mpd_status_class_init();

	window = new(mtk_window, WIDTH, HEIGHT);
	call(window,add_widget, mtk_widget(new(mpd_main)));

	mtk_main();

	mtk_cleanup();

	return 0;
}
