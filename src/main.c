#include <time.h>
#include <config.h>
#include <charlie.h>
#include <mtk.h>

int main (int argc, char *argv[])
{
	mtk_window_t *window;
	mtk_widget_t *mpdlist;
	mtk_list_t* dir;

	mtk_init();
	mpd_init();

	dir = mpd_get_dir("");

	window = mtk_window_new(WIDTH, HEIGHT);
	mpdlist = mtk_mpdlist_new(0, 0, WIDTH, HEIGHT, dir);
	mtk_window_add(window, mpdlist);

	mtk_main();

	mtk_cleanup();

	return 0;
}
