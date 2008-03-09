#include <stdlib.h>
#include <string.h>
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

mtk_list_t* mpd_get_dir(const char *dir)
{
	mpd_InfoEntity *entity;
	mtk_list_t *list = mtk_list_new();

	mpd_sendLsInfoCommand(conn, dir);
	die_on_mpd_error();

	while((entity = mpd_getNextInfoEntity(conn)))
		mtk_list_append(list, entity);

	die_on_mpd_error();

	mpd_finishCommand(conn);
	die_on_mpd_error();

	return list;
}
