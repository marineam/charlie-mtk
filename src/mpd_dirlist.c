#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <charlie.h>

/* Extend the mpd_Directory structure to include a name in
 * addition to just it's path.
 * This is setup in make_dir_name() or set_dir_name()
 * be sure to call free_dir_name() on the dir before calling
 * free_InfoEntity or free_Directory */
typedef struct {
	mpd_Directory;
	char *name;
} Directory;

#define MAXTEXT 200
static char* filename(char *path)
{
	static char name[MAXTEXT], *s;
	int len;

	s = strrchr(path, '/');
	s = s ? s+1 : path;

	strncpy(name, s, MAXTEXT);

	len = strlen(name);
	for (int i = 0; i < len; i++) {
		if (name[i] == '_')
			name[i] = ' ';
	}

	return name;
}

static void set_dir_name(mpd_InfoEntity *entity, char *name)
{
	Directory **dir = (Directory**)&entity->info.directory;

	*dir = xrealloc(*dir, sizeof(Directory));
	(*dir)->name = strdup(name);
}

static void make_dir_name(mpd_InfoEntity *entity)
{
	if (entity->type != MPD_INFO_ENTITY_TYPE_DIRECTORY)
		return;

	set_dir_name(entity, filename(entity->info.directory->path));
}

static void free_dir_name(mpd_InfoEntity *entity)
{
	Directory *dir = (Directory*)entity->info.directory;

	if (entity->type != MPD_INFO_ENTITY_TYPE_DIRECTORY)
		return;

	free(dir->name);
}

static char* _item_text(void *this, void *item)
{
	char *name;
	mpd_InfoEntity *entity = item;
	Directory *dir;

	switch(entity->type) {
		case MPD_INFO_ENTITY_TYPE_SONG:
			if (entity->info.song->title)
				name = entity->info.song->title;
			else
				name = filename(entity->info.song->file);
			break;
		case MPD_INFO_ENTITY_TYPE_DIRECTORY:
			dir = (Directory*)entity->info.directory;

			/* dir->name should always get set by set_dir_name */
			assert(dir->name);
			name = dir->name;

			break;
		case MPD_INFO_ENTITY_TYPE_PLAYLISTFILE:
			name = filename(entity->info.playlistFile->path);
			break;
		default:
			assert(0);
	}

	return name;
}

static void updatedir(mpd_dirlist_t *this, char *path)
{
	mpd_InfoEntity *entity;
	mtk_list_t* list = mtk_list_new();

	if (path && strlen(path)) {
		/* set p to the parent directory or "" */
		char *p = strdup(path);
		char *e = strrchr(p, '/');
		if (e)
			*e = '\0';
		else
			*p = '\0';

		entity = mpd_newInfoEntity();
		entity->type = MPD_INFO_ENTITY_TYPE_DIRECTORY;
		entity->info.directory = mpd_newDirectory();
		entity->info.directory->path = p;
		set_dir_name(entity, "<- Parent Directory");
		mtk_list_append(list, entity);
	}
	else
		path = "";

	mpd_sendLsInfoCommand(mpd_conn, path);
	die_on_mpd_error();

	while((entity = mpd_getNextInfoEntity(mpd_conn))) {
		die_on_mpd_error();
		make_dir_name(entity);
		mtk_list_append(list, entity);
	}

	mpd_finishCommand(mpd_conn);
	die_on_mpd_error();

	call(this,set_list, list);
}

static void update(void *vthis)
{
}

static void _item_draw(void *vthis, cairo_t *cr, void *item, int y)
{
        mpd_dirlist_t *this = vthis;
	mtk_widget_t *widget = vthis;

	super(this,mpd_dirlist,_item_draw, cr, item, y);

	cairo_set_source_rgb(cr, 0.6, 0.6, 0.9);
	cairo_rectangle(cr, widget->w - 2*UNIT + UNIT*0.1, y + UNIT*0.1,
		2*UNIT-UNIT*0.2, UNIT*0.8);
	cairo_fill(cr);

	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_move_to(cr, widget->w - UNIT, y + UNIT*0.3);
	cairo_line_to(cr, widget->w - UNIT, y + UNIT*0.7);
	cairo_move_to(cr, widget->w - UNIT*1.2, y + UNIT*0.5);
	cairo_line_to(cr, widget->w - UNIT*0.8, y + UNIT*0.5);
	cairo_stroke(cr);
}

static void _item_click(void *vthis, void *item, int x, int y)
{
	mpd_dirlist_t *this = vthis;
	mpd_InfoEntity *entity = item;

	if (entity->type == MPD_INFO_ENTITY_TYPE_DIRECTORY) {
		if (x >= mtk_widget(this)->w - UNIT*2) {
			mpd_sendAddCommand(mpd_conn, entity->info.directory->path);
			mpd_finishCommand(mpd_conn);
			die_on_mpd_error();
		}
		else
			updatedir(this, entity->info.directory->path);
	}
	else if (entity->type == MPD_INFO_ENTITY_TYPE_SONG) {
		if (x >= mtk_widget(this)->w - UNIT*2) {
			mpd_sendAddCommand(mpd_conn, entity->info.directory->path);
			mpd_finishCommand(mpd_conn);
			die_on_mpd_error();
		}
		else if (0) { // fixme: check if song is in playlist
			mpd_sendPlayCommand(mpd_conn, entity->info.song->pos);
			mpd_finishCommand(mpd_conn);
			die_on_mpd_error();
		}
	}
}

static void _item_free(void *this, void *item)
{
	free_dir_name(item);
	mpd_freeInfoEntity((mpd_InfoEntity*)item);
}

static void objfree(void *vthis)
{
	mtk_text_list_t *this = vthis;
	mpd_InfoEntity *entity;

	mtk_list_foreach(this->list, entity) {
		free_dir_name(entity);
		mpd_freeInfoEntity(entity);
	}

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
	METHOD(_item_draw);
	METHOD(_item_click);
	METHOD(_item_free);
METHOD_TABLE_END
