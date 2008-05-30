#ifndef CHARLIE_H
#define CHARLIE_H

#include <libmpdclient.h>
#include "mtk.h"

CLASS(mpd_dirlist, mtk_text_list)
METHODS(mpd_dirlist, mtk_text_list)
END

CLASS(mpd_playlist, mtk_text_list)
METHODS(mpd_playlist, mtk_text_list)
	void (*update)(void *this);
END

CLASS(mpd_status, mtk_container)
	mpd_Status *status;
	mtk_text_t *title;
	mtk_text_t *artist;
	mtk_text_t *album;
	mtk_text_t *elapsed;
	mtk_text_t *remaining;
	mtk_image_t *art;
METHODS(mpd_status, mtk_container)
END

extern mpd_Connection *mpd_conn;
#define die_on_mpd_error() die_on(mpd_conn->error, "%s\n", mpd_conn->errorStr)

void mpd_init();

#endif
