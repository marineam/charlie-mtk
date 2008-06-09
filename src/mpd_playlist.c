#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <charlie.h>

static char* _item_text(void *this, void *item)
{
	return mpd_song_name((mpd_InfoEntity*)item);
}

static void update(void *vthis)
{
	mpd_playlist_t *this = vthis;
	mpd_InfoEntity *entity;
	mtk_list_t* list;

	if (mpd_stat->playlist == this->playlist &&
	    mpd_stat->song == this->song) {
		return;
	}

	list = mtk_list_new();
	this->playlist = mpd_stat->playlist;
	this->song = mpd_stat->song;

	/* FIXME: do an incremental update instead */
	mpd_sendPlaylistInfoCommand(mpd_conn, -1);
	die_on_mpd_error();

	while ((entity = mpd_getNextInfoEntity(mpd_conn))) {
		die_on_mpd_error();
		mtk_list_append(list, entity);
	}

	mpd_finishCommand(mpd_conn);
	die_on_mpd_error();

	call(this,set_list, list);
}

static void _item_click(void *vthis, void *item, int x, int y)
{
	mpd_InfoEntity *entity = item;

	assert(entity->type == MPD_INFO_ENTITY_TYPE_SONG);

	mpd_sendPlayCommand(mpd_conn, entity->info.song->pos);
	die_on_mpd_error();
	mpd_finishCommand(mpd_conn);
	die_on_mpd_error();
}

static void _item_free(void *this, void *item)
{
	mpd_freeInfoEntity((mpd_InfoEntity*)item);
}

static void objfree(void *vthis)
{
	mtk_text_list_t *this = vthis;
	mpd_InfoEntity *entity;

	mtk_list_foreach(this->list, entity)
		mpd_freeInfoEntity(entity);

	mtk_list_free(this->list);

	/* skip over mtk_text_list's free */
	super(this,mtk_text_list,free);
}

mpd_playlist_t* mpd_playlist_new(size_t size)
{
	mpd_playlist_t *this = mpd_playlist(mtk_text_list_new(size, NULL));

	SET_CLASS(this, mpd_playlist);

	return this;
}

METHOD_TABLE_INIT(mpd_playlist, mtk_text_list)
	_METHOD(free, objfree);
	METHOD(_item_text);
	METHOD(_item_click);
	METHOD(_item_free);
	METHOD(update);
METHOD_TABLE_END
