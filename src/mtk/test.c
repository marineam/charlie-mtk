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

	mtk_init();

	window = mtk_window_new(640, 480);
	clickarea = mtk_clickarea_new(0, 0, 640, 480);
	mtk_window_add(window, clickarea);

	mtk_timer_add(0.5, timer, "I'm a timer!");

	mtk_main();

	mtk_cleanup();

	return 0;
}
