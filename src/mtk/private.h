#ifndef _PRIVATE_H
#define _PRIVATE_H

#include <mtk.h>

extern int _screen_num;
extern xcb_connection_t *_conn;
extern xcb_screen_t *_screen;
extern xcb_visualtype_t *_visual;
extern mtk_list_t *_windows;

void _mtk_flush();

void _mtk_timer_init();
int _mtk_timer_event();

#endif
