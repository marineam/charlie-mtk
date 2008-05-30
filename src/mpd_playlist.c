#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <charlie.h>

#define MAXTEXT 200
static char* _item_text(void *this, void *item)
{
	static char name[MAXTEXT];
	mpd_InfoEntity *entity = item;
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

static void update(void *vthis)
{
	mpd_playlist_t *this = vthis;
	mpd_InfoEntity *entity;
	mtk_list_t* list = mtk_list_new();

	mpd_sendPlaylistInfoCommand(conn, -1);
	die_on_mpd_error();

	while ((entity = mpd_getNextInfoEntity(conn))) {
		die_on_mpd_error();
		mtk_list_append(list, entity);
	}

	mpd_finishCommand(conn);
	die_on_mpd_error();

	call(this,set_list, list);
}

static void _item_click(void *vthis, void *item)
{
	mpd_InfoEntity *entity = item;

	assert(entity->type == MPD_INFO_ENTITY_TYPE_SONG);

	mpd_sendPlayCommand(conn, entity->info.song->pos);
	die_on_mpd_error();
	mpd_finishCommand(conn);
	die_on_mpd_error();
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
	call(this, update);

	return this;
}

METHOD_TABLE_INIT(mpd_playlist, mtk_text_list)
	_METHOD(free, objfree);
	METHOD(_item_text);
	METHOD(_item_click);
	METHOD(update);
METHOD_TABLE_END
