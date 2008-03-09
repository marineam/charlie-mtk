#include <time.h>
#include <mtk.h>

int main (int argc, char *argv[])
{
	mtk_window_t *window;
	/* pause for a 100th of a second between polls */
	struct timespec pause = {.tv_sec = 0, .tv_nsec = 1000000};

	mtk_init();

	window = mtk_window_new(640, 480);

	while (1) {
		if (mtk_event() < 0)
			break;
		nanosleep(&pause,NULL);
	}

	return 0;
}
