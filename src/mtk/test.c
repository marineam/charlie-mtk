#include <mtk.h>

int timer(void * data)
{
	printf("%s\n", (char*)data);
	return 1;
}

int main (int argc, char *argv[])
{
	mtk_window_t *window;
	mtk_vpack_t *vpack;
	mtk_text_t *txt1;
	mtk_text_t *txt2;

	mtk_init();

	window = new(mtk_window,640, 480);
	vpack = new(mtk_vpack);

	call(window, mtk_container, add_widget, mtk_widget(vpack));

	txt1 = new(mtk_text, "I'm on the top");
	txt2 = new(mtk_text, "The bottom is sooo much better");

	call(vpack, mtk_vpack, pack_bottom, mtk_widget(txt1), 0);
	call(vpack, mtk_vpack, pack_bottom, mtk_widget(txt2), 0);

	mtk_timer_add(0.5, timer, "I'm a timer!");

	mtk_main();

	mtk_cleanup();

	return 0;
}
