#ifndef CHARLIE_H
#define CHARLIE_H

#include "mtk.h"

/* mpd.c */
void mpd_init();
mtk_list_t* mpd_get_dir(const char *dir);

#endif
