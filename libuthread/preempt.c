#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

struct sigaction prev_sigaction;
struct itimerval prev_itimerval;

void preempt_disable(void)
{
	// Initilize
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGVTALRM);

	// Disable preemption by blocking the SIGVTALRM signal
	sigprocmask(SIG_BLOCK, &set, NULL);
}

void preempt_enable(void)
{
	// Initilize
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGVTALRM);

	// Enable preemption by unblocking the SIGVTALRM signal
	sigprocmask(SIG_UNBLOCK, &set, NULL);
}

void sighandler(int sig)
{
	(void)sig;
	uthread_yield();
}

void preempt_start(bool preempt)
{
	if (!preempt)
		return;

    // Set the timer to send SIGVTALRM every 1/HZ seconds
    struct itimerval curr_itimerval;
    curr_itimerval.it_interval.tv_sec = 0;
    curr_itimerval.it_interval.tv_usec = 1000000 / HZ;
    curr_itimerval.it_value.tv_sec = 0;
    curr_itimerval.it_value.tv_usec = 1000000 / HZ;
    setitimer(ITIMER_VIRTUAL, &curr_itimerval, NULL);
	
	// Set the sighandler to run yield on current thread
	struct sigaction curr_sigaction;
	curr_sigaction.sa_handler = sighandler;
	sigaction(SIGVTALRM, &curr_sigaction, NULL);
}

void preempt_stop(void)
{
	// Restore prev configuration
	setitimer(ITIMER_VIRTUAL, &prev_itimerval, NULL);
	sigaction(SIGVTALRM, &prev_sigaction, NULL);
}
