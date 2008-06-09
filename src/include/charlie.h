#ifndef CHARLIE_H
#define CHARLIE_H

#include <libmpdclient.h>
#include "mtk.h"

/* Default window size */
#define WIDTH 800
#define HEIGHT 480

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
	mtk_text_t *vollabel;
	mtk_slider_t *volume;
	mtk_text_t *volvalue;
	mtk_image_t *art;
	mtk_image_t *playpause;
	mtk_text_t *nextlabel;
	mtk_text_t *next[3];
	long long playlist;
	int song;
METHODS(mpd_status, mtk_container)
	void (*update)(void *this);
	void (*set_progress)(void *this, double value);
	void (*set_volume)(void *this, double value);
	void (*set_playpause)(void *this);
END

extern mpd_Connection *mpd_conn;
extern mpd_Status *mpd_stat;
#define die_on_mpd_error() die_on(mpd_conn->error, "%s\n", mpd_conn->errorStr)

void mpd_init();
char* mpd_song_name(mpd_InfoEntity *entity);

#endif
