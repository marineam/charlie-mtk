#ifndef CHARLIE_H
#define CHARLIE_H

#include <libmpdclient.h>
#include "mtk.h"

CLASS(mpd_dirlist, mtk_text_list)
METHODS(mpd_dirlist, mtk_text_list)
	void (*update)(void *this);
END

CLASS(mpd_playlist, mtk_text_list)
	long long playlist;
	int song;
METHODS(mpd_playlist, mtk_text_list)
	void (*update)(void *this);
END

CLASS(mpd_status, mtk_container)
	mtk_text_t *title;
	mtk_text_t *artist;
	mtk_text_t *album;
	mtk_text_t *elapsed;
	mtk_slider_t *progress;
	mtk_text_t *remaining;
	mtk_image_t *art;
	long long playlist;
	int song;
METHODS(mpd_status, mtk_container)
	void (*update)(void *this);
END

extern mpd_Connection *mpd_conn;
extern mpd_Status *mpd_stat;
#define die_on_mpd_error() die_on(mpd_conn->error, "%s\n", mpd_conn->errorStr)

void mpd_init();

#endif
