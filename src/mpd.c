#include <stdlib.h>
#include <string.h>
#include "libmpdclient/libmpdclient.h"
#include "util.h"
#include "mpd.h"

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

static void fill_tree(GtkTreeStore *tree)
{
	mpd_InfoEntity *entity;
	GtkTreeIter parent_pos;
	char *parent_name = NULL;

	//mpd_sendListCommand(conn,MPD_TABLE_ARTIST,NULL);
	mpd_sendListallInfoCommand(conn, "/");
	die_on_mpd_error();

	while((entity = mpd_getNextInfoEntity(conn))) {
		GtkTreeIter pos;
		char *name;
		switch (entity->type) {
			case MPD_INFO_ENTITY_TYPE_DIRECTORY:
				name = entity->info.directory->path;
				break;
			case MPD_INFO_ENTITY_TYPE_SONG:
				name = entity->info.song->file;
				break;
			case MPD_INFO_ENTITY_TYPE_PLAYLISTFILE:
				name = entity->info.playlistFile->path;
		}

		name = strdup(name); // how do we free this?
		if (parent_name && strstr(name, parent_name)) {
			printf("%s parent of %s\n", parent_name, name);
			gtk_tree_store_append(tree, &pos, &parent_pos);
		}
		else
			gtk_tree_store_append(tree, &pos, NULL);

		gtk_tree_store_set(tree, &pos, NAME_COLUMN, name, -1);

		if (entity->type == MPD_INFO_ENTITY_TYPE_DIRECTORY) {
			if (parent_name)
				free(parent_name);
			asprintf(&parent_name, "%s/", name);
			memcpy(&parent_pos, &pos, sizeof(parent_pos));
		}
		mpd_freeInfoEntity(entity);
	}
	die_on_mpd_error();

	if (parent_name)
		free(parent_name);

	mpd_finishCommand(conn);
	die_on_mpd_error();
}

GtkWidget* mpd_tree_new()
{
   GtkTreeStore *store;
   GtkWidget *tree, *scrollbox;
   GtkTreeViewColumn *column;
   GtkCellRenderer *renderer;

   /* Create a model.  We are using the store model for now, though we
    * could use any other GtkTreeModel */
   store = gtk_tree_store_new (N_COLUMNS, G_TYPE_STRING);

   /* custom function to fill the model with data */
   fill_tree(store);

   /* Create a view */
   tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));

   /* The view now holds a reference.  We can get rid of our own
    * reference */
   g_object_unref (G_OBJECT (store));

   gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE);

   /* Create a cell render and arbitrarily make it red for demonstration
    * purposes */
   renderer = gtk_cell_renderer_text_new ();
   g_object_set (G_OBJECT (renderer),
                 "foreground", "red",
                 NULL);

   /* Create a column, associating the "text" attribute of the
    * cell_renderer to the first column of the model */
   gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree),
		-1, NULL, renderer, "text", NAME_COLUMN, NULL);

   /* Now we can manipulate the view just like any other GTK widget */
   scrollbox = gtk_scrolled_window_new(NULL, NULL);
   gtk_container_add(GTK_CONTAINER(scrollbox), tree);

   return scrollbox;
}

