#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <charlie.h>

#define MAXTEXT 200
static char* _item_text(void *this, void *item)
{
	static char name[MAXTEXT];
	mpd_InfoEntity *entity = item;
	char *file = NULL;

	switch(entity->type) {
		case MPD_INFO_ENTITY_TYPE_SONG:
			if (entity->info.song->title)
				strncpy(name,
					entity->info.song->title, MAXTEXT);
			else
				file = entity->info.song->file;
			break;
		case MPD_INFO_ENTITY_TYPE_DIRECTORY:
			file = entity->info.directory->path;
			break;
		case MPD_INFO_ENTITY_TYPE_PLAYLISTFILE:
			file = entity->info.playlistFile->path;
			break;
		default:
			assert(0);
	}

	if (file) {
		char *s;
		int len;

		s = strrchr(file, '/');
		s = s?s+1:file;
		if (strstr(file, "../") == file)
			snprintf(name, MAXTEXT, "Directory: %s", s);
		else
			strncpy(name, s, MAXTEXT);

		len = strlen(name);
		for (int i = 0; i < len; i++) {
			if (name[i] == '_')
				name[i] = ' ';
		}
	}

	return name;
}

static void updatedir(mpd_dirlist_t *this, mpd_InfoEntity *dir)
{
	mpd_InfoEntity *entity;
	mtk_list_t* list = mtk_list_new();
	mtk_list_t* playlist = mtk_list_new(); //temp
	char *path;

	if (dir) {
		mtk_list_append(list, dir);
		if (strstr(dir->info.directory->path, "../") ==
		    dir->info.directory->path)
			/* ../ indicates go to parent directory of path */
			path = dir->info.directory->path+3;
		else
			path = dir->info.directory->path;
	}
	else
		path = "";

	mpd_sendLsInfoCommand(mpd_conn, path);
	die_on_mpd_error();

	while((entity = mpd_getNextInfoEntity(mpd_conn))) {
		die_on_mpd_error();
		mtk_list_append(list, entity);
		// temp
		if (entity->type == MPD_INFO_ENTITY_TYPE_SONG)
			mtk_list_append(playlist, entity);
	}

	// temp
	if (mtk_list_length(playlist)) {
		int pos = 0;
		mpd_sendCommandListBegin(mpd_conn);
		mpd_sendClearCommand(mpd_conn);
		mtk_list_foreach(playlist, entity) {
			entity->info.song->pos = pos;
			mpd_sendAddIdCommand(mpd_conn, entity->info.song->file);
			pos++;
		}
		mpd_sendCommandListEnd(mpd_conn);
	}
	mtk_list_free(playlist);

	die_on_mpd_error();

	mpd_finishCommand(mpd_conn);
	die_on_mpd_error();

	call(this,set_list, list);
}

static void update(void *vthis)
{
}

static void _item_click(void *vthis, void *item)
{
	mpd_dirlist_t *this = vthis;
	mpd_InfoEntity *entity = item, *new = NULL;
	char *path;

	if (entity->type == MPD_INFO_ENTITY_TYPE_DIRECTORY) {
		if (strstr(entity->info.directory->path, "../") ==
		    entity->info.directory->path) {
			/* ../ indicates go to parent directory of path */
			char *p;
			path = strdup(entity->info.directory->path+3);
			p = strrchr(path, '/');
			if (p)
				*p = '\0';
			else {
				free(path);
				path = NULL;
			}
		}
		else
			path = strdup(entity->info.directory->path);

		if (path) {
			new = mpd_newInfoEntity();
			new->type = MPD_INFO_ENTITY_TYPE_DIRECTORY;
			new->info.directory = mpd_newDirectory();
			asprintf(&new->info.directory->path, "../%s", path);
			free(path);
		}

		updatedir(this, new);
	}
	else if (entity->type == MPD_INFO_ENTITY_TYPE_SONG) {
		mpd_sendPlayCommand(mpd_conn, entity->info.song->pos);
		die_on_mpd_error();
		mpd_finishCommand(mpd_conn);
		die_on_mpd_error();
	}
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

mpd_dirlist_t* mpd_dirlist_new(size_t size)
{
	mpd_dirlist_t *this = mpd_dirlist(mtk_text_list_new(size, NULL));

	SET_CLASS(this, mpd_dirlist);
	updatedir(this, NULL);

	return this;
}

METHOD_TABLE_INIT(mpd_dirlist, mtk_text_list)
	METHOD(update);
	_METHOD(free, objfree);
	METHOD(_item_text);
	METHOD(_item_click);
METHOD_TABLE_END
