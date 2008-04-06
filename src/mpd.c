#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libmpdclient.h>
#include <charlie.h>
#include <mtk.h>

static mpd_Connection *conn;

#define die_on_mpd_error() die_on(conn->error, "%s\n", conn->errorStr)

static int updatestatus(void *nill)
{
	mpd_Status *status;

	mpd_sendStatusCommand(conn);
	status = mpd_getStatus(conn);
	die_on_mpd_error();

	mpd_freeStatus(status);
	mpd_finishCommand(conn);
	die_on_mpd_error();

	return 1;
}

void mpd_init()
{
	char *hostname = getenv("MPD_HOST");
	char *port = getenv("MPD_PORT");

	if(hostname == NULL)
		hostname = "localhost";
	if(port == NULL)
		port = "6600";

	conn = mpd_newConnection(hostname, atoi(port), 10);
	die_on_mpd_error();

	mtk_timer_add(1.0, updatestatus, NULL);
}

static void updatedir(mtk_list_t *list, void *data)
{
	mpd_InfoEntity *entity, *old, *dir = data;
	mtk_list_t* playlist = mtk_list_new();
	char *path;
	int pos;

	if (dir)
		if (strstr(dir->info.directory->path, "../") ==
		    dir->info.directory->path)
			/* ../ indicates go to parent directory of path */
			path = dir->info.directory->path+3;
		else
			path = dir->info.directory->path;
	else
		path = "";

	mpd_sendLsInfoCommand(conn, path);
	die_on_mpd_error();

	/* Empty dir list, we'll just replace it to be simple */
	mtk_list_goto(list, 0);
	while ((old = mtk_list_remove(list))) {
		if (old != data)
			mpd_freeInfoEntity(old);
	}

	if (dir)
		mtk_list_append(list, dir);

	while((entity = mpd_getNextInfoEntity(conn))) {
		die_on_mpd_error();
		mtk_list_append(list, entity);
		if (entity->type == MPD_INFO_ENTITY_TYPE_SONG)
			mtk_list_append(playlist, entity);
	}

	if (mtk_list_length(playlist)) {
		mpd_sendCommandListBegin(conn);
		mpd_sendClearCommand(conn);
		pos = 0;
		mtk_list_foreach(playlist, entity) {
			entity->info.song->pos = pos;
			mpd_sendAddIdCommand(conn, entity->info.song->file);
			pos++;
		}
		mpd_sendCommandListEnd(conn);
	}

	die_on_mpd_error();

	mpd_finishCommand(conn);
	die_on_mpd_error();
}

static int clicked(void **data, mtk_list_t *list, int pos)
{
	mpd_InfoEntity *entity, *new;
	char *path;

	entity = mtk_list_goto(list, pos);
	assert(entity);
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
			*data = new;
			free(path);
		}
		else {
			*data = NULL;
		}

		return 1;
	}
	else if (entity->type == MPD_INFO_ENTITY_TYPE_SONG) {
		mpd_sendPlayCommand(conn, entity->info.song->pos);
		die_on_mpd_error();
		mpd_finishCommand(conn);
		die_on_mpd_error();
	}

	return 0;
}

mtk_widget_t* mpd_dirlist_new(int x, int y, int w, int h) {
	return mtk_mpdlist_new(x, y, w, h, updatedir, clicked, NULL);
}

mtk_widget_t* mpd_status_new(int x, int y, int w, int h) {
	return mtk_text_new(x, y, w, UNIT, "test");
}
