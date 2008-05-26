#ifndef CHARLIE_H
#define CHARLIE_H

#include <libmpdclient.h>
#include "mtk.h"

/* mpd.c */
CLASS(mpd_dirlist, mtk_mpdlist)
METHODS(mpd_dirlist, mtk_mpdlist)
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

extern mpd_Connection *conn;
#define die_on_mpd_error() die_on(conn->error, "%s\n", conn->errorStr)

void mpd_init();

#endif
