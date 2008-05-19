#include <time.h>
#include <config.h>
#include <charlie.h>
#include <mtk.h>

static mtk_window_t *window;
static mtk_viewer_t *view;
static mpd_dirlist_t *mpdlist;
static mpd_status_t *mpdstatus;

bool doit(void *nill)
{
	static bool base = false;

	if (base)
		call(view,mtk_viewer,slide_in,mtk_widget(mpdlist));
	else
		call(view,mtk_viewer,slide_in,mtk_widget(mpdstatus));

	base = !base;

	return true;
}

int main (int argc, char *argv[])
{
	mtk_init();
	mpd_init();
	_mpd_dirlist_class_init();
	_mpd_status_class_init();

	window = new(mtk_window, WIDTH, HEIGHT);
	view = new(mtk_viewer);
	call(window,mtk_container,add_widget, mtk_widget(view));
	mpdlist = new(mpd_dirlist);
	mpdstatus = new(mpd_status);
	call(view,mtk_container,add_widget, mtk_widget(mpdlist));
	call(view,mtk_container,add_widget, mtk_widget(mpdstatus));

	mtk_timer_add(5.0, doit, NULL);
	mtk_main();

	mtk_cleanup();

	return 0;
}
