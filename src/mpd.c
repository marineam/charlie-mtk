#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
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

		if (mpd_stat->state == MPD_STATUS_STATE_PLAY)
			call(this->playpause,set_image, "../data/pause.png");
		else
			call(this->playpause,set_image, "../data/play.png");
	}

	if (old)
		mpd_freeStatus(old);

	/* update the clock */
	time_t t;
	struct tm tm;
	char timestr[10];

	t = time(NULL);
	localtime_r(&t, &tm);
	strftime(timestr, sizeof(timestr), "%l:%M %p", &tm);
	call(this->clock,set_text, timestr);

	return true;
}

static void set_playpause(void *vthis)
{
	mpd_main_t *this = vthis;

	if (mpd_stat->state == MPD_STATUS_STATE_PLAY)
		mpd_sendPauseCommand(mpd_conn, 1);
	else if (mpd_stat->state == MPD_STATUS_STATE_PAUSE)
		mpd_sendPauseCommand(mpd_conn, 0);
	else
		mpd_sendPlayCommand(mpd_conn, mpd_stat->song);

	mpd_finishCommand(mpd_conn);
	die_on_mpd_error();
	mpd_update(this);
}

static void draw(void *vthis)
{
	mpd_main_t *this = vthis;
	cairo_t *cr = cairo_create(mtk_widget(this)->surface);

	cairo_rectangle(cr, 0, 0, 3*UNIT, mtk_widget(this)->w);
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_fill(cr);

	cairo_destroy(cr);

	super(this,mpd_main,draw);
}


static void set_size(void *vthis, int w, int h)
{
	mpd_main_t *this = vthis;

	super(this,mpd_main,set_size, w, h);

	assert(w > 3*UNIT);
	call(this->menu,set_size, w-3*UNIT, h);

	call(this->clock,set_coord, UNIT*0.1, h-UNIT*0.6);
	call(this->clock,set_size, UNIT*2.8, UNIT*0.5);

	call(this->playpause,set_coord, UNIT*0.1, h-UNIT*1.6);
	call(this->playpause,set_size, UNIT*0.8, UNIT*0.8);
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
	this->playpause = new(mtk_image, "../data/play.png");
	this->clock = new(mtk_text, "00:00 XM");
	call(this->menu,set_coord, 3*UNIT, 0);
	call(this,add_widget, mtk_widget(this->menu));
	call(this,add_widget, mtk_widget(this->playpause));
	call(this,add_widget, mtk_widget(this->clock));
	call(this->menu,add_item, mtk_widget(this->status), "Current Song");
	call(this->menu,add_item, mtk_widget(this->playlist), "Playlist");
	call(this->menu,add_item, mtk_widget(this->dirlist), "Music Library");

	mpd_update(this);
	mtk_timer_add(1.0, mpd_update, this);

	connect(mtk_widget(this->playpause), clicked, this, set_playpause);

	return this;
}

METHOD_TABLE_INIT(mpd_main, mtk_container)
	METHOD(draw);
	METHOD(set_size);
	METHOD(set_playpause);
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
