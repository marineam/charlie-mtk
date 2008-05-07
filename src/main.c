#include <time.h>
#include <config.h>
#include <charlie.h>
#include <mtk.h>

int main (int argc, char *argv[])
{
	mtk_window_t *window;
	mpd_dirlist_t *mpdlist;
	mpd_status_t *mpdstatus;

	mtk_init();
	mpd_init();
	_mpd_dirlist_class_init();
	_mpd_status_class_init();

	window = new(mtk_window, WIDTH, HEIGHT);
	mpdlist = new(mpd_dirlist, WIDTH*0.35, 0, WIDTH*0.65, HEIGHT);
	mpdstatus = new(mpd_status, 0, 0, WIDTH*0.35, HEIGHT);
	call(window,mtk_container,add_widget, mtk_widget(mpdlist));
	call(window,mtk_container,add_widget, mtk_widget(mpdstatus));

	mtk_main();

	mtk_cleanup();

	return 0;
}
