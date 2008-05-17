#include <time.h>
#include <config.h>
#include <charlie.h>
#include <mtk.h>

int main (int argc, char *argv[])
{
	mtk_window_t *window;
	mtk_hpack_t *hpack;
	mpd_dirlist_t *mpdlist;
	mpd_status_t *mpdstatus;

	mtk_init();
	mpd_init();
	_mpd_dirlist_class_init();
	_mpd_status_class_init();

	window = new(mtk_window, WIDTH, HEIGHT);
	hpack = new(mtk_hpack);
	call(window,mtk_container,add_widget, mtk_widget(hpack));
	mpdlist = new(mpd_dirlist);
	mpdstatus = new(mpd_status);
	call(hpack,mtk_hpack,pack_left, mtk_widget(mpdlist), 0);
	call(hpack,mtk_hpack,pack_left, mtk_widget(mpdstatus), UNIT*5);

	mtk_main();

	mtk_cleanup();

	return 0;
}
