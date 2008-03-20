
#include <assert.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
//#include <limits.h>
#include <string.h>
#include <mtk.h>
#include "private.h"

struct timer {
	timer_t id;
	int active;
	void *data;
	int (*callback)(void *data);
};

static mtk_list_t *timers;
static mtk_list_t *events;

static void timer_handler(int sig, siginfo_t *info, void *data)
{
	assert(info->si_ptr);
	mtk_list_append(events, info->si_ptr);
}

void _mtk_timer_init()
{
	struct sigaction handler;

	timers = mtk_list_new();
	events = mtk_list_new();

	handler.sa_sigaction = timer_handler;
	handler.sa_flags = SA_SIGINFO;
	sigemptyset(&handler.sa_mask);

	sigaction(SIGALRM, &handler, NULL);
}

int _mtk_timer_event()
{
	sigset_t sigset;
	struct timer *t;
	int ret = 0;

	/* block SIGALRM while we handle events
	 * this is important since mtk_lists don't have any locking */
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);
	sigprocmask(SIG_BLOCK, &sigset, NULL);

	mtk_list_goto(events, 0);
	if ((t = mtk_list_remove(events))) {
		if (!t->callback(t->data)) {
			t->active = 0;
			timer_delete(t->id);
		}
		ret = 1;
	}
	else {
		/* there were no pending events so it should
		 * now be safe to free unactive timers */
		t = mtk_list_goto(timers, 0);
		while (t) {
			if (!t->active) {
				free(t);
				mtk_list_remove(timers);
				t = mtk_list_current(timers);
			}
			else {
				t = mtk_list_next(timers);
			}
		}
	}

	sigprocmask(SIG_UNBLOCK, &sigset, NULL);
	return ret;
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
	t->active = 1;
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

