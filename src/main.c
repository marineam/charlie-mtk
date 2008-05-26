#include <time.h>
#include <config.h>
#include <charlie.h>
#include <mtk.h>

int main (int argc, char *argv[])
{
	mtk_window_t *window;
	mtk_menu_t *view;
	mpd_dirlist_t *mpdlist;
	mpd_status_t *mpdstatus;

	mtk_init();
	mpd_init();
	_mpd_dirlist_class_init();
	_mpd_status_class_init();

	window = new(mtk_window, WIDTH, HEIGHT);
	view = new(mtk_menu);
	call(window,add_widget, mtk_widget(view));
	mpdlist = new(mpd_dirlist);
	mpdstatus = new(mpd_status);
	call(view,add_item, mtk_widget(mpdstatus), "Currently Playing");
	call(view,add_item, mtk_widget(mpdlist), "Music Library");

	mtk_main();

	mtk_cleanup();

	return 0;
}
