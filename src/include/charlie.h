#ifndef CHARLIE_H
#define CHARLIE_H

#include "mtk.h"

/* mpd.c */
CLASS(mpd_dirlist, mtk_mpdlist)
METHODS(mpd_dirlist, mtk_mpdlist, int x, int y, int w, int h)
END

CLASS(mpd_status, mtk_container)
	mtk_text_t *title;
	mtk_text_t *artist;
	mtk_text_t *album;
METHODS(mpd_status, mtk_container, int x, int y, int w, int h)
END

void mpd_init();

#endif
