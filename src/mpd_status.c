#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libmpdclient.h>
#include <charlie.h>
#include <cairo.h>
#include <mtk.h>

static bool updatestatus(void *data)
{
	mpd_Status *status;
	mpd_InfoEntity *entity;
	mpd_status_t *widget = data;

	mpd_sendStatusCommand(conn);

	status = mpd_getStatus(conn);
	die_on_mpd_error();
	assert(status);

	if (widget->status) {
		if (widget->status->playlist == status->playlist &&
		    widget->status->song == status->song) {
			if (widget->status->state == status->state &&
			    status->state != MPD_STATUS_STATE_PLAY) {
				/* nothing important changed so do nothing */
				goto do_nothing;
			}
			else {
				/* Not much changed, just update the time */
				goto do_redraw;
			}
		}
		else {
			mpd_freeStatus(widget->status);
		}
	}

	mpd_sendCurrentSongCommand(conn);
	entity = mpd_getNextInfoEntity(conn);
	if (entity && entity->type == MPD_INFO_ENTITY_TYPE_SONG &&
	   (status->state == MPD_STATUS_STATE_PLAY ||
	    status->state == MPD_STATUS_STATE_PAUSE) ) {
		mpd_Song *song = entity->info.song;

		if (song->title)
			call(widget->title,set_text, song->title);
		else
			call(widget->title,set_text, song->file);
		if (song->artist)
			call(widget->artist,set_text, song->artist);
		else
			call(widget->artist,set_text, "");
		if (song->album)
			call(widget->album,set_text, song->album);
		else
			call(widget->album,set_text, "");
	}
	else {
		call(widget->title,set_text, "");
		call(widget->artist,set_text, "");
		call(widget->album,set_text, "");
	}

	if (entity)
		mpd_freeInfoEntity(entity);

do_redraw:
	widget->status = status;
	call(widget,redraw);

do_nothing:
	mpd_finishCommand(conn);
	die_on_mpd_error();

	return true;
}

static void draw(void *vthis)
{
	mtk_widget_t *this = vthis;
	mpd_Status *status = mpd_status(vthis)->status;
	cairo_t *cr = cairo_create(this->surface);
	double x;

	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_rectangle(cr, 0, 0, this->w, this->h);
	cairo_fill(cr);

	super(this,mpd_status,draw);

	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	cairo_move_to(cr, this->w*0.25, this->h*0.75);
	cairo_line_to(cr, this->w*0.75, this->h*0.75);
	cairo_stroke(cr);

	x = ((double)status->elapsedTime / status->totalTime) *
		(this->w*0.5) + this->w*0.25;

	cairo_move_to(cr, x, this->h*0.75 - 5);
	cairo_line_to(cr, x, this->h*0.75 + 5);
	cairo_stroke(cr);

	cairo_destroy(cr);
}

static void set_size(void *vthis, int w, int h)
{
	mpd_status_t *this = vthis;

	/* skip mtk container */
	super(this,mtk_container,set_size, w, h);

	call(this->art,set_coord, w*0.1, h*0.1);
	call(this->art,set_size, w*0.3, h*0.8);

	call(this->title,set_coord, w*0.45, h*0.4);
	call(this->title,set_size, w*0.5, h*0.08);

	call(this->artist,set_coord, w*0.45, h*0.5);
	call(this->artist,set_size, w*0.5, h*0.06);

	call(this->album,set_coord, w*0.45, h*0.56);
	call(this->album,set_size, w*0.5, h*0.06);
}

mpd_status_t* mpd_status_new(size_t size)
{
	mpd_status_t *this = mpd_status(mtk_container_new(size));

	SET_CLASS(this, mpd_status);

	this->art = new(mtk_image, "album-test.png");
	this->title = new(mtk_text, "");
	this->artist = new(mtk_text, "");
	this->album = new(mtk_text, "");

	call(this,add_widget, mtk_widget(this->art));
	call(this,add_widget, mtk_widget(this->title));
	call(this,add_widget, mtk_widget(this->artist));
	call(this,add_widget, mtk_widget(this->album));

	updatestatus(this);
	mtk_timer_add(1.0, updatestatus, this);

	return this;
}

METHOD_TABLE_INIT(mpd_status, mtk_container)
	METHOD(draw);
	METHOD(set_size);
METHOD_TABLE_END
