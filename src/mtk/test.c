#include <mtk.h>

int timer(void * data)
{
	printf("%s\n", (char*)data);
	return 1;
}

int main (int argc, char *argv[])
{
	mtk_window_t *window;
	mtk_widget_t *widget;

	mtk_init();

	window = new(mtk_window,640, 480);
	widget = mtk_widget(new(mtk_text, 0, 0, 640, 480, "WHEE"));
	call(window, mtk_container, add_widget, widget);

	mtk_timer_add(0.5, timer, "I'm a timer!");

	mtk_main();

	mtk_cleanup();

	return 0;
}
