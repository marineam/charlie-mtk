
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <mtk.h>
#include "private.h"

struct timer {
	timer_t id;
	double interval;
	double remaining;
	void *data;
	int (*callback)(void *data);
};

static mtk_list_t *timers;
static pthread_t timer_thread_id;

/* This thread will spend all of it's time sleeping,
 * only responding when a signal comes in. */
static void* timer_thread(void* unused)
{
	siginfo_t info;
	sigset_t sigset;

	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);

	while (sigwaitinfo(&sigset, &info) > 0) {
		struct timer *t = info.si_ptr;

		assert(t);
		t->callback(t->data);
		/* todo: check return status */
	}

	fprintf(stderr, "Timer thread got unknown signal!");
	exit(1);

	return NULL;
}

void _mtk_timer_init()
{
	sigset_t sigset;
	int r;

	timers = mtk_list_new();

	/* block SIGALRM in main thread */
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);
	pthread_sigmask(SIG_BLOCK, &sigset, NULL);

	r = pthread_create(&timer_thread_id, NULL, timer_thread, NULL);
	die_on(r<0, "pthread_create failded\n");
}

void mtk_timer_add(double interval, int(*callback)(void *data), void *data)
{
	struct timer *t = xmalloc(sizeof(struct timer));
	struct sigevent sigev;
	struct itimerspec time;

	assert(interval > 0);
	assert(callback);

	/* setup a sigevent structure so we can pass the pointer to
	 * struct timer directly to the signal handler */
	memset(&sigev, 0, sizeof(sigev));
	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = SIGALRM;
	sigev.sigev_value.sival_ptr = t;

	timer_create(CLOCK_REALTIME, &sigev, &t->id);
	t->interval = interval;
	t->remaining = interval;
	t->callback = callback;
	t->data = data;

	mtk_list_append(timers, t);

	time.it_interval.tv_sec = (time_t)interval;
	time.it_interval.tv_nsec = (long)(interval*1000000000);
	assert(time.it_interval.tv_sec || time.it_interval.tv_nsec);
	time.it_value.tv_sec = time.it_interval.tv_sec;
	time.it_value.tv_nsec = time.it_interval.tv_nsec;
	timer_settime(t->id, 0, &time, NULL);
}

