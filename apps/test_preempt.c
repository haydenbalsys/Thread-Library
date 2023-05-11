#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "private.h"
#include "uthread.h"

int count;

void thread1(void* arg)
{
    (void) arg;
    printf("Context switch to thread1\n");
    printf("Thread looped %d times before timer was triggered\n", count);
    raise(SIGINT);
}

void thread(void* arg)
{
    (void) arg;
    uthread_create(thread1, NULL);
    while (1) {
        count++;
        // SLeep to allow count to increment
        usleep(5); 
    }
}

int main(void)
{
	uthread_run(true, thread, NULL);
	return 0;
}