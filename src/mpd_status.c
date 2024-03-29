#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libmpdclient.h>
#include <charlie.h>
#include <cairo.h>
#include <mtk.h>
#include <openssl/md5.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static void set_time(mtk_text_t *widget, int time)
{
	char *str;

	if (time >= 0)
		asprintf(&str, "%d:%02d", abs(time / 60), abs(time % 60));
	else
		asprintf(&str, "-%d:%02d", abs(time / 60), abs(time % 60));
	assert(str);
	call(widget,set_text, str);
	free(str);
}

/* This is kinda screwy but to keep things easy I'm ripping
 * album art out of amarok and it likes to hash art by the function:
 * md5(tolower(artist)tolower(album))
 *
 * result must be freed
 */
static char* hashpath(const char *path, const char *artist, const char *album)
{
	unsigned char hash[MD5_DIGEST_LENGTH];
	int pathlen = strlen(path);
	int len = strlen(artist) + strlen(album);
	char str[len+1], *ret;

	snprintf(str, len+1, "%s%s", artist, album);

	for (int i = 0; i < len; i++) {
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] += 'a' - 'A';
	}

	MD5((unsigned char*)str, len, hash);

	ret = xmalloc(pathlen+2*MD5_DIGEST_LENGTH+1);
	strncpy(ret, path, pathlen);
	for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
		snprintf(ret+pathlen+2*i, 3, "%02x", hash[i]);

	return ret;
}

static void set_art(mtk_image_t *widget, mpd_Song *song)
{
	const char path[] = "../data/";
	struct stat statdata;
	char *str;

	if (!song || !song->artist || !song->album) {
		/* TODO: fix paths */
		call(widget,set_image,"../data/album.png");
		return;
	}

	str = hashpath(path, song->artist, song->album);
	if (stat(str, &statdata) < 0) {
		/* stat failed, maybe it's a mixed artist album */
		free(str);
		str = hashpath(path, "", song->album);
		if (stat(str, &statdata) < 0) {
			/* still no good, set default */
			free(str);
			call(widget,set_image,"../data/album.png");
			return;
		}
	}

	call(widget,set_image,str);
	free(str);
}

static void update(void *vthis)
{
	mpd_InfoEntity *entity = NULL;
	mpd_status_t *this = vthis;
	char vol[5];

	set_time(this->elapsed, mpd_stat->elapsedTime);
	set_time(this->remaining, mpd_stat->elapsedTime - mpd_stat->totalTime);
	if (mpd_stat->totalTime)
		call(this->progress,set_value,
			(double)mpd_stat->elapsedTime / mpd_stat->totalTime);
	else
		call(this->progress,set_value, 0.0);

	call(this->volume,set_value, (double)mpd_stat->volume / 100.0);
	if (mpd_stat->volume == MPD_STATUS_NO_VOLUME)
		vol[0] = '\0';
	else
		snprintf(vol, 5, "%d%%", mpd_stat->volume);
	call(this->volvalue,set_text, vol);

	if (mpd_stat->playlist == this->playlist &&
	    mpd_stat->song == this->song) {
		call(this,redraw);
		return;
	}

	this->playlist = mpd_stat->playlist;
	this->song = mpd_stat->song;

	if (mpd_stat->playlistLength) {
		mpd_sendCommandListBegin(mpd_conn);
		mpd_sendPlaylistInfoCommand(mpd_conn, mpd_stat->song);
		mpd_sendPlaylistInfoCommand(mpd_conn, mpd_stat->song+1);
		mpd_sendPlaylistInfoCommand(mpd_conn, mpd_stat->song+2);
		mpd_sendPlaylistInfoCommand(mpd_conn, mpd_stat->song+3);
		mpd_sendCommandListEnd(mpd_conn);
		entity = mpd_getNextInfoEntity(mpd_conn);
	}

	if (entity) {
		mpd_Song *song = entity->info.song;
		assert(entity->type == MPD_INFO_ENTITY_TYPE_SONG);

		if (song->title)
			call(this->title,set_text, song->title);
		else
			call(this->title,set_text, song->file);
		if (song->artist)
			call(this->artist,set_text, song->artist);
		else
			call(this->artist,set_text, "");
		if (song->album)
			call(this->album,set_text, song->album);
		else
			call(this->album,set_text, "");
		set_art(this->art, song);
		mpd_freeInfoEntity(entity);
	}
	else {
		call(this->title,set_text, "");
		call(this->artist,set_text, "");
		call(this->album,set_text, "");
		set_art(this->art, NULL);
	}

	for (int i = 0; i < 3; i++) {
		entity = mpd_getNextInfoEntity(mpd_conn);
		if (entity) {
			call(this->next[i],set_text, mpd_song_name(entity));
			mpd_freeInfoEntity(entity);
		}
		else {
			call(this->next[i],set_text, "");
			mpd_clearError(mpd_conn);
		}

	}

	mpd_finishCommand(mpd_conn);
	die_on_mpd_error();

	call(this,redraw);
}

static void draw(void *vthis)
{
	mtk_widget_t *this = vthis;
	cairo_t *cr = cairo_create(this->surface);

	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_rectangle(cr, 0, 0, this->w, this->h);
	cairo_fill(cr);

	cairo_destroy(cr);

	super(this,mpd_status,draw);
}

static void set_size(void *vthis, int w, int h)
{
	mpd_status_t *this = vthis;

	/* skip mtk container */
	super(this,mtk_container,set_size, w, h);

	call(this->art,set_coord, w*0.05, h*0.05);
	call(this->art,set_size, w*0.3, h*0.5);

	call(this->title,set_coord, w*0.4, h*0.1);
	call(this->title,set_size, w*0.6, h*0.08);

	call(this->artist,set_coord, w*0.4, h*0.2);
	call(this->artist,set_size, w*0.5, h*0.06);

	call(this->album,set_coord, w*0.4, h*0.26);
	call(this->album,set_size, w*0.5, h*0.06);

	call(this->elapsed,set_coord, w*0.4, h*0.4);
	call(this->elapsed,set_size, w*0.06, h*0.04);

	call(this->progress,set_coord, w*0.46, h*0.4);
	call(this->progress,set_size, w*0.42, h*0.04);

	call(this->remaining,set_coord, w*0.89, h*0.4);
	call(this->remaining,set_size, w*0.11, h*0.04);

	call(this->vollabel,set_coord, w*0.4, h*0.5);
	call(this->vollabel,set_size, w*0.06, h*0.04);

	call(this->volume,set_coord, w*0.46, h*0.5);
	call(this->volume,set_size, w*0.42, h*0.04);

	call(this->volvalue,set_coord, w*0.89, h*0.5);
	call(this->volvalue,set_size, w*0.11, h*0.04);

	call(this->nextlabel,set_coord, w*0.05, h*0.6);
	call(this->nextlabel,set_size, w*0.2, h*0.06);

	call(this->next[0],set_coord, w*0.05, h*0.67);
	call(this->next[0],set_size, w*0.9, h*0.06);
	call(this->next[1],set_coord, w*0.05, h*0.74);
	call(this->next[1],set_size, w*0.9, h*0.06);
	call(this->next[2],set_coord, w*0.05, h*0.81);
	call(this->next[2],set_size, w*0.9, h*0.06);
}

static void set_progress(void *vthis, double value)
{
	mpd_status_t *this = vthis;

	mpd_stat->elapsedTime = mpd_stat->totalTime*value;

	mpd_sendSeekCommand(mpd_conn, mpd_stat->song, mpd_stat->elapsedTime);
	mpd_finishCommand(mpd_conn);
	die_on_mpd_error();
	call(this,update);
}

static void set_volume(void *vthis, double value)
{
	mpd_status_t *this = vthis;

	mpd_stat->volume = value*100;

	mpd_sendSetvolCommand(mpd_conn, mpd_stat->volume);
	mpd_finishCommand(mpd_conn);
	die_on_mpd_error();
	call(this,update);
}

mpd_status_t* mpd_status_new(size_t size)
{
	mpd_status_t *this = mpd_status(mtk_container_new(size));

	SET_CLASS(this, mpd_status);

	/* TODO: fix path */
	this->art = new(mtk_image, "../data/album.png");
	this->title = new(mtk_text, "");
	this->artist = new(mtk_text, "");
	this->album = new(mtk_text, "");
	this->elapsed = new(mtk_text, "");
	this->progress = new(mtk_slider, 0.0);
	this->remaining = new(mtk_text, "");
	this->vollabel = new(mtk_text, "Vol:");
	this->volume = new(mtk_slider, 0.0);
	this->volvalue = new(mtk_text, "0%");
	this->nextlabel = new(mtk_text, "Next:");
	this->next[0] = new(mtk_text, "");
	this->next[1] = new(mtk_text, "");
	this->next[2] = new(mtk_text, "");

	call(this,add_widget, mtk_widget(this->art));
	call(this,add_widget, mtk_widget(this->title));
	call(this,add_widget, mtk_widget(this->artist));
	call(this,add_widget, mtk_widget(this->album));
	call(this,add_widget, mtk_widget(this->elapsed));
	call(this,add_widget, mtk_widget(this->progress));
	call(this,add_widget, mtk_widget(this->remaining));
	call(this,add_widget, mtk_widget(this->vollabel));
	call(this,add_widget, mtk_widget(this->volume));
	call(this,add_widget, mtk_widget(this->volvalue));
	call(this,add_widget, mtk_widget(this->nextlabel));
	call(this,add_widget, mtk_widget(this->next[0]));
	call(this,add_widget, mtk_widget(this->next[1]));
	call(this,add_widget, mtk_widget(this->next[2]));

	connect(this->progress, value_changed, this, set_progress);
	connect(this->volume, value_changed, this, set_volume);

	return this;
}

METHOD_TABLE_INIT(mpd_status, mtk_container)
	METHOD(update);
	METHOD(set_progress);
	METHOD(set_volume);
	METHOD(draw);
	METHOD(set_size);
METHOD_TABLE_END
