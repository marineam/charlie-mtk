#ifndef _PRIVATE_H
#define _PRIVATE_H

#include <mtk.h>

extern int _screen_num;
extern xcb_connection_t *_conn;
extern xcb_screen_t *_screen;
extern xcb_visualtype_t *_visual;
extern mtk_list_t *_windows;

void _mtk_flush();

void _mtk_window_draw(mtk_window_t *window);
void _mtk_window_mouse_press(mtk_window_t *window, int x, int y);
void _mtk_window_mouse_release(mtk_window_t *window, int x, int y);
void _mtk_window_mouse_move(mtk_window_t *window, int x, int y);
void _mtk_window_resize(mtk_window_t *window, int w, int h);

void _mtk_timer_init();
int _mtk_timer_event();

#endif
