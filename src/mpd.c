#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <libmpdclient.h>
#include <charlie.h>

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

enum
{
   NAME_COLUMN,
   N_COLUMNS
};

GList* mpd_get_dir(const char *dir)
{
	mpd_InfoEntity *entity;
	GList *list = NULL;

	mpd_sendLsInfoCommand(conn, dir);
	die_on_mpd_error();

	while((entity = mpd_getNextInfoEntity(conn)))
		list = g_list_append(list, entity);

	die_on_mpd_error();

	mpd_finishCommand(conn);
	die_on_mpd_error();

	return list;
}
