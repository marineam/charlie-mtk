
#include <assert.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <mtk.h>
#include "private.h"

/* for some reason this is provided for timeval but not timespec */
# define timescmp(a, b, CMP) 						      \
  (((a)->tv_sec == (b)->tv_sec) ? 					      \
   ((a)->tv_nsec CMP (b)->tv_nsec) : 					      \
   ((a)->tv_sec CMP (b)->tv_sec))

struct timer {
	timer_t id;
	int active;
	void *data;
	int (*callback)(void *data);
	struct timespec lastfire;
};

static mtk_list_t *timers;
static mtk_list_t *events;

static void event(void *data)
{
	struct timer *t = data;

	mtk_timer_block();

	if (t->active && !t->callback(t->data)) {
		t->active = 0;
		timer_delete(t->id);
		clock_gettime(CLOCK_REALTIME, &t->lastfire);
	}
}

static void timer_handler(int sig, siginfo_t *info, void *data)
{
	/* Only handle signals from timers */
	if (info->si_code == SI_TIMER) {
		assert(info->si_ptr);
		mtk_event_add(event, info->si_ptr);
	}
}

void _mtk_timer_init()
{
	struct sigaction handler;

	timers = mtk_list_new();
	events = mtk_list_new();

	handler.sa_sigaction = timer_handler;
	handler.sa_flags = SA_SIGINFO;
	sigemptyset(&handler.sa_mask);

	sigaction(TIMER_SIG, &handler, NULL);
}

void _mtk_timer_cleanup()
{
	struct timer *t;
	struct timespec now;

	clock_gettime(CLOCK_REALTIME, &now);
	t = mtk_list_goto(timers, 0);
	while (t) {
		if (!t->active && timescmp(&t->lastfire, &now, <)) {
			free(t);
			mtk_list_remove(timers);
			t = mtk_list_current(timers);
		}
		else {
			t = mtk_list_next(timers);
		}
	}
}

void mtk_timer_block()
{
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, TIMER_SIG);
	sigprocmask(SIG_BLOCK, &sigset, NULL);
}

void mtk_timer_unblock()
{
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, TIMER_SIG);
	sigprocmask(SIG_UNBLOCK, &sigset, NULL);
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
	sigev.sigev_signo = TIMER_SIG;
	sigev.sigev_value.sival_ptr = t;

	timer_create(CLOCK_REALTIME, &sigev, &t->id);
	t->active = 1;
	t->callback = callback;
	t->data = data;

	mtk_list_append(timers, t);

	time.it_interval.tv_sec = (time_t)interval;
	time.it_interval.tv_nsec =
		(long)((interval-(time_t)interval)*1000000000);
	assert(time.it_interval.tv_sec || time.it_interval.tv_nsec);
	time.it_value.tv_sec = time.it_interval.tv_sec;
	time.it_value.tv_nsec = time.it_interval.tv_nsec;
	timer_settime(t->id, 0, &time, NULL);
}

