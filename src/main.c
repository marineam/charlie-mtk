#include <time.h>
#include <config.h>
#include <charlie.h>
#include <mtk.h>

int main (int argc, char *argv[])
{
	mtk_window_t *window;
	mtk_widget_t *mpdlist;
	mtk_widget_t *mpdstatus;

	mtk_init();
	mpd_init();

	window = mtk_window_new(WIDTH, HEIGHT);
	mpdlist = mpd_dirlist_new(WIDTH*0.35, 0, WIDTH*0.65, HEIGHT);
	mpdstatus = mpd_status_new(0, 0, WIDTH*0.35, HEIGHT);
	mtk_window_add(window, mpdlist);
	mtk_window_add(window, mpdstatus);

	mtk_main();

	mtk_cleanup();

	return 0;
}
