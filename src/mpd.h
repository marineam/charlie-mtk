#ifndef MPD_H
#define MPD_H

#include <glib.h>

void mpd_init();
GList* mpd_get_dir(const char *dir);

#endif
