#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libmpdclient.h>
#include <charlie.h>
#include <mtk.h>

mpd_Connection *mpd_conn;
mpd_Status *mpd_stat;

static bool mpd_update(void *vthis)
{
	mpd_main_t *this = vthis;
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
		/* Not much changed, just update the time/volume */
		call(this->status,update);
	}
	else {
		call(this->status,update);
		call(this->playlist,update);
		call(this->dirlist,update);
	}

	if (old)
		mpd_freeStatus(old);

	return true;
}


mpd_main_t* mpd_main_new(size_t size)
{
	mpd_main_t *this = mpd_main(mtk_container_new(size));
	char *hostname = getenv("MPD_HOST");
	char *port = getenv("MPD_PORT");

	SET_CLASS(this, mpd_main);

	if(hostname == NULL)
		hostname = "localhost";
	if(port == NULL)
		port = "6600";

	mpd_conn = mpd_newConnection(hostname, atoi(port), 10);
	die_on_mpd_error();

	this->menu = new(mtk_menu);
	this->status = new(mpd_status);
	this->playlist = new(mpd_playlist);
	this->dirlist = new(mpd_dirlist);
	call(this,add_widget, mtk_widget(this->menu));
	call(this->menu,add_item, mtk_widget(this->status), "Current Song");
	call(this->menu,add_item, mtk_widget(this->playlist), "Playlist");
	call(this->menu,add_item, mtk_widget(this->dirlist), "Music Library");

	mpd_update(this);
	mtk_timer_add(1.0, mpd_update, this);

	return this;
}

METHOD_TABLE_INIT(mpd_main, mtk_container)
METHOD_TABLE_END

#define MAXTEXT 200
char* mpd_song_name(mpd_InfoEntity *entity)
{
	static char name[MAXTEXT];
	mpd_Song *song = entity->info.song;

	assert(entity->type == MPD_INFO_ENTITY_TYPE_SONG);
	if (song->title && song->artist)
		snprintf(name, MAXTEXT, "%s - %s", song->artist, song->title);
	else {
		char *s;
		s = strrchr(song->file, '/');
		s = s?s+1:song->file;
		strncpy(name, s, MAXTEXT);
	}

	return name;
}
