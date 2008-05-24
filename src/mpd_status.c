#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libmpdclient.h>
#include <charlie.h>
#include <mtk.h>

static bool updatestatus(void *data)
{
	mpd_Status *status;
	mpd_InfoEntity *entity;
	mpd_status_t *widget = data;

	mpd_sendCommandListOkBegin(conn);
	mpd_sendStatusCommand(conn);
	mpd_sendCurrentSongCommand(conn);
	mpd_sendCommandListEnd(conn);

	status = mpd_getStatus(conn);
	die_on_mpd_error();

	mpd_nextListOkCommand(conn);

	entity = mpd_getNextInfoEntity(conn);
	if (entity && entity->type == MPD_INFO_ENTITY_TYPE_SONG) {
		mpd_Song *song = entity->info.song;

		if (song->title)
			call(widget->title,set_text, song->title);
		else
			call(widget->title,set_text, song->file);
		if (song->artist)
			call(widget->artist,set_text, song->artist);
		if (song->album)
			call(widget->album,set_text, song->album);
	}

	if (entity)
		mpd_freeInfoEntity(entity);

	mpd_freeStatus(status);
	mpd_finishCommand(conn);
	die_on_mpd_error();

	return true;
}

static void set_size(void *vthis, int w, int h)
{
	mpd_status_t *this = vthis;

	/* skip mtk container */
	super(this,mtk_container,set_size, w, h);

	call(this->title,set_size, w-UNIT, UNIT*0.5);
	call(this->artist,set_size, w-UNIT, UNIT*0.5);
	call(this->album,set_size, w-UNIT, UNIT*0.5);
}

mpd_status_t* mpd_status_new(size_t size)
{
	mpd_status_t *this = mpd_status(mtk_container_new(size));

	SET_CLASS(this, mpd_status);

	this->art = new(mtk_image, "album-test.png");
	call(this->art,set_coord, 0, 0);
	call(this->art,set_size, UNIT*1.5, UNIT*1.5);
	call(this,add_widget, mtk_widget(this->art));

	this->title = new(mtk_text, "");
	this->artist = new(mtk_text, "");
	this->album = new(mtk_text, "");
	call(this->title,set_coord, UNIT*1.5, 0);
	call(this->artist,set_coord, UNIT*1.5, UNIT*0.5);
	call(this->album,set_coord, UNIT*1.5, UNIT);
	call(this,add_widget, mtk_widget(this->title));
	call(this,add_widget, mtk_widget(this->artist));
	call(this,add_widget, mtk_widget(this->album));

	updatestatus(this);
	mtk_timer_add(1.0, updatestatus, this);

	return this;
}

METHOD_TABLE_INIT(mpd_status, mtk_container)
	METHOD(set_size);
METHOD_TABLE_END
