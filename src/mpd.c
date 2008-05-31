#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libmpdclient.h>
#include <charlie.h>
#include <mtk.h>

mpd_Connection *mpd_conn;
mpd_Status *mpd_stat;
static mpd_status_t *status;
static mpd_playlist_t *playlist;
static mpd_dirlist_t *dirlist;

static bool mpd_update(void *nill)
{
	mpd_Status *old;

	mpd_sendStatusCommand(mpd_conn);

	old = mpd_stat;
	mpd_stat = mpd_getStatus(mpd_conn);
	die_on_mpd_error();
	assert(mpd_stat);

	if (old &&
	    mpd_stat->playlist == old->playlist &&
	    mpd_stat->song == old->song &&
	    mpd_stat->state == old->state) {
		if (mpd_stat->state == MPD_STATUS_STATE_PLAY) {
			/* Not much changed, just update the time */
			call(status,update);
		}
	}
	else {
		call(status,update);
		call(playlist,update);
		call(dirlist,update);
	}

	if (old)
		mpd_freeStatus(old);

	return true;
}

void mpd_init(mtk_menu_t *view)
{
	char *hostname = getenv("MPD_HOST");
	char *port = getenv("MPD_PORT");

	if(hostname == NULL)
		hostname = "localhost";
	if(port == NULL)
		port = "6600";

	mpd_conn = mpd_newConnection(hostname, atoi(port), 10);
	die_on_mpd_error();

	status = new(mpd_status);
	playlist = new(mpd_playlist);
	dirlist = new(mpd_dirlist);
	call(view,add_item, mtk_widget(status), "Currently Playing");
	call(view,add_item, mtk_widget(playlist), "Playlist");
	call(view,add_item, mtk_widget(dirlist), "Music Library");

	mpd_update(NULL);
	mtk_timer_add(1.0, mpd_update, NULL);
}
