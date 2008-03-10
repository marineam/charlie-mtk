#include <time.h>
#include <mtk.h>

int timer(void * data)
{
	printf("%s\n", (char*)data);
	return 1;
}

int main (int argc, char *argv[])
{
	mtk_window_t *window;
	mtk_widget_t *clickarea;
	/* pause for a 100th of a second between polls */
	struct timespec pause = {.tv_sec = 0, .tv_nsec = 1000000};

	mtk_init();

	window = mtk_window_new(640, 480);
	clickarea = mtk_clickarea_new(0, 0, 640, 480);
	mtk_window_add(window, clickarea);

	mtk_timer_add(0.5, timer, "I'm a timer!");

	while (1) {
		if (mtk_event() < 0)
			break;
		nanosleep(&pause,NULL);
	}

	return 0;
}
