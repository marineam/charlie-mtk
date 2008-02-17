#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>

#define die_on(x, fmt, args...) \
	if (x) { \
		fprintf(stderr, fmt, ## args); \
		exit(1); \
	}

#endif
