#include <time.h>
#include <config.h>
#include <charlie.h>
#include <mtk.h>

int main (int argc, char *argv[])
{
	mtk_window_t *window;
	mtk_widget_t *mpdlist;
	mtk_list_t* dir;
	/* pause for a 100th of a second between polls */
	struct timespec pause = {.tv_sec = 0, .tv_nsec = 1000000};

	mtk_init();
	mpd_init();

	dir = mpd_get_dir("");

	window = mtk_window_new(WIDTH, HEIGHT);
	mpdlist = mtk_mpdlist_new(0, 0, WIDTH, HEIGHT, dir);
	mtk_window_add(window, mpdlist);

	while (1) {
		if (mtk_event() < 0)
			break;
		nanosleep(&pause,NULL);
	}

	return 0;
}
