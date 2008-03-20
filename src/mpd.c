#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libmpdclient.h>
#include <charlie.h>
#include <mtk.h>

static mpd_Connection *conn;

#define die_on_mpd_error() die_on(conn->error, "%s\n", conn->errorStr)

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
}

static void updatedir(mtk_list_t *list, void *data)
{
	mpd_InfoEntity *entity, *old;

	mpd_sendLsInfoCommand(conn, (char*)data);
	die_on_mpd_error();

	mtk_list_goto(list, 0);
	while((entity = mpd_getNextInfoEntity(conn))) {
		die_on_mpd_error();
		old = mtk_list_replace(list, entity);
		mtk_list_next(list);
		//if (old)
		//	mpd_freeInfoEntity(old);
	}

	while (old) {
		old = mtk_list_remove(list);
		//mpd_freeInfoEntity(old);
	}

	die_on_mpd_error();

	mpd_finishCommand(conn);
	die_on_mpd_error();
}

static int clicked(void **data, mtk_list_t *list, int pos)
{
	mpd_InfoEntity *entity;

	entity = mtk_list_goto(list, pos);
	assert(entity);
	if (entity->type == MPD_INFO_ENTITY_TYPE_DIRECTORY) {
		printf("%s\n", entity->info.directory->path);
		free(*data);
		*data = strdup(entity->info.directory->path);
	}
	return 1;
}

mtk_widget_t* mpd_dirlist_new(int x, int y, int w, int h) {
	return mtk_mpdlist_new(x, y, w, h, updatedir, clicked, strdup(""));
}
