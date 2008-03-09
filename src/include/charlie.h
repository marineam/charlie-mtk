#ifndef CHARLIE_H
#define CHARLIE_H

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

/* mpd.c */
void mpd_init();
GList* mpd_get_dir(const char *dir);

#endif
