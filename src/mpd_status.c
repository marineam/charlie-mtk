#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libmpdclient.h>
#include <charlie.h>
#include <cairo.h>
#include <mtk.h>
#include <openssl/md5.h>

static void set_time(mtk_text_t *widget, int time)
{
	char *str;

	asprintf(&str, "%d:%02d", time / 60, abs(time % 60));
	assert(str);
	call(widget,set_text, str);
	free(str);
}

/* This is kinda screwy but to keep things easy I'm ripping
 * album art out of amarok and it likes to hash art by the function:
 * md5(tolower(artist)tolower(album))
 */
static void set_art(mtk_image_t *widget, mpd_Song *song)
{
	unsigned char hash[MD5_DIGEST_LENGTH];
	const char path[] = "../data/";
	int start = strlen(path);
	char *str;

	if (!song || !song->artist || !song->album) {
		/* TODO: fix paths */
		call(widget,set_image,"../data/album.png");
		return;
	}

	asprintf(&str, "%s%s", song->artist, song->album);
	for (int i = 0; i < strlen(str); i++) {
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] += 'a' - 'A';
	}
	MD5((unsigned char*)str, strlen(str), hash);
	free(str);

	str = xmalloc(start+2*MD5_DIGEST_LENGTH+1);
	strncpy(str, path, start+1);

	for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
		snprintf(str+start+2*i, 3, "%02x", hash[i]);

	//printf("setting image to: %s\n", str);
	call(widget,set_image,str);
}

static void update(void *vthis)
{
	mpd_InfoEntity *entity = NULL;
	mpd_status_t *this = vthis;

	set_time(this->elapsed, mpd_stat->elapsedTime);
	set_time(this->remaining, mpd_stat->elapsedTime - mpd_stat->totalTime);

	if (mpd_stat->playlist == this->playlist &&
	    mpd_stat->song == this->song) {
		call(this,redraw);
		return;
	}

	this->playlist = mpd_stat->playlist;
	this->song = mpd_stat->song;

	if (mpd_stat->playlistLength) {
		mpd_sendPlaylistInfoCommand(mpd_conn, mpd_stat->song);
		entity = mpd_getNextInfoEntity(mpd_conn);
		mpd_finishCommand(mpd_conn);
		die_on_mpd_error();
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
	}
	else {
		call(this->title,set_text, "");
		call(this->artist,set_text, "");
		call(this->album,set_text, "");
		set_art(this->art, NULL);
	}

	if (entity)
		mpd_freeInfoEntity(entity);

	call(this,redraw);
}

static void draw(void *vthis)
{
	mtk_widget_t *this = vthis;
	cairo_t *cr = cairo_create(this->surface);
	double x;

	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_rectangle(cr, 0, 0, this->w, this->h);
	cairo_fill(cr);

	super(this,mpd_status,draw);

	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	cairo_move_to(cr, this->w*0.46, this->h*0.42);
	cairo_line_to(cr, this->w*0.88, this->h*0.42);
	cairo_stroke(cr);

	x = ((double)mpd_stat->elapsedTime / mpd_stat->totalTime) *
		(this->w*(0.88-0.46)) + this->w*0.46;

	cairo_move_to(cr, x, this->h*0.41);
	cairo_line_to(cr, x, this->h*0.43);
	cairo_stroke(cr);

	cairo_destroy(cr);
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

	call(this->remaining,set_coord, w*0.89, h*0.4);
	call(this->remaining,set_size, w*0.11, h*0.04);
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
	this->remaining = new(mtk_text, "");

	call(this,add_widget, mtk_widget(this->art));
	call(this,add_widget, mtk_widget(this->title));
	call(this,add_widget, mtk_widget(this->artist));
	call(this,add_widget, mtk_widget(this->album));
	call(this,add_widget, mtk_widget(this->elapsed));
	call(this,add_widget, mtk_widget(this->remaining));

	return this;
}

METHOD_TABLE_INIT(mpd_status, mtk_container)
	METHOD(update);
	METHOD(draw);
	METHOD(set_size);
METHOD_TABLE_END
