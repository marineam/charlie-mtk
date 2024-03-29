#ifndef _PRIVATE_H
#define _PRIVATE_H

#include <mtk.h>
#include <X11/Xlib.h>

#define TIMER_SIG SIGRTMIN

extern int _screen;
extern Display *_display;
extern Visual *_visual;
extern mtk_list_t *_windows;
extern Atom _delete_window;

void _mtk_event_init();
bool _mtk_event();
void _mtk_timer_init();
void _mtk_timer_cleanup();

#endif
