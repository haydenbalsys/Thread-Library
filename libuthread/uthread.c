#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

/* Globals */
queue_t ready_Q;
queue_t zombie_Q;
struct uthread_tcb *curr_thread;
struct uthread_tcb *idle_thread;

typedef enum state
{
	running,
	ready,
	blocked,
	zombie
} state_t;

struct uthread_tcb
{
	state_t state;
	void *stack;
	uthread_ctx_t *context;
};

struct uthread_tcb *uthread_current(void)
{
	return curr_thread;
}

void uthread_yield(void)
{
	// Disable preemption
	preempt_disable();
	struct uthread_tcb *next_thread;
	struct uthread_tcb *current = uthread_current();

	// Put the current thread back on the queue and get the next thread
	queue_enqueue(ready_Q, curr_thread);
	queue_dequeue(ready_Q, (void **)&next_thread);

	if (next_thread == NULL)
		perror("queue_dequeue");

	// State change
	curr_thread->state = ready;
	next_thread->state = running;

	// Swap contexts
	curr_thread = next_thread;
	uthread_ctx_switch(current->context, next_thread->context);
	preempt_enable();
}

void uthread_exit(void)
{
	// Set the current thread to exited and remove it
	curr_thread->state = zombie;
	uthread_ctx_destroy_stack(curr_thread->stack);
	queue_enqueue(zombie_Q, uthread_current());
	// yield to the next thread
	struct uthread_tcb *next_thread;
	queue_dequeue(ready_Q, (void **)&next_thread);

	// set the next thread to running and swap contexts
	struct uthread_tcb *curr = curr_thread;
	curr_thread = next_thread;
	curr_thread->state = running;
	uthread_ctx_switch(curr->context, next_thread->context);
}

int uthread_create(uthread_func_t func, void *arg)
{
	// Make a new thread
	struct uthread_tcb *new_thread = malloc(sizeof(struct uthread_tcb));
	// Check for thread allocation failure
	if (new_thread == NULL)
		return -1;

	// Prep the thread
	new_thread->state = ready;
	new_thread->stack = uthread_ctx_alloc_stack();
	// Check for stack allocateion failure
	if (new_thread->stack == NULL)
		return -1;

	// Allocate the context
	new_thread->context = malloc(sizeof(uthread_ctx_t));
	if (new_thread->context == NULL)
		return -1;

	// ret = 0 if success, -1 if failure
	int ret = uthread_ctx_init(new_thread->context, new_thread->stack, func, arg);
	if (ret == -1)
		return -1;

	// Add thread to the ready_Q
	queue_enqueue(ready_Q, new_thread);
	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	// Preempt check
	if (preempt)
		preempt_start(preempt);

	// Initialize the ready_Q and zombie_Q
	ready_Q = queue_create();
	zombie_Q = queue_create();
	// Check for queue allocation failure
	if (ready_Q == NULL || zombie_Q == NULL)
		return -1;

	// Create the main thread
	idle_thread = malloc(sizeof(struct uthread_tcb));
	// Check for thread allocation failure
	if (idle_thread == NULL)
		return -1;
	// Prep the thread
	idle_thread->state = running;
	// idle_thread->stack = uthread_ctx_alloc_stack();
	idle_thread->context = malloc(sizeof(uthread_ctx_t));
	// Check for allocation failures
	if (idle_thread->state != running || idle_thread->context == NULL)
		return -1;

	// Make the idle thread the first thread we "run"
	curr_thread = idle_thread;

	// Check for thread creation failure
	if (uthread_create(func, arg))
		return -1;

	// Handle Qs and execution
	while (1)
	{
		// Check for completed threads
		while (queue_length(zombie_Q) > 0)
		{
			// Deallocate the dead threads
			struct uthread_tcb *exited_thread;
			queue_dequeue(zombie_Q, (void **)&exited_thread);
			free(exited_thread);
		}

		// Check if all the threads are completed
		if (queue_length(ready_Q) <= 0)
			break;

		uthread_yield();
	}

	if (preempt)
		preempt_stop();

	return 0;
}

void uthread_block(void)
{
	curr_thread->state = blocked;

	// Get the next thread from the ready_Q
	struct uthread_tcb *next_thread;
	queue_dequeue(ready_Q, (void **)&next_thread);

	if (next_thread == NULL)
		perror("queue_dequeue");

	// Prep thread
	next_thread->state = running;

	// Swap contexts
	struct uthread_tcb *curr = curr_thread;
	curr_thread = next_thread;
	uthread_ctx_switch(curr->context, next_thread->context);
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	if (uthread == NULL)
		return;

	// Set the thread to ready and put it on the ready_Q
	uthread->state = ready;
	queue_enqueue(ready_Q, uthread);

	// Yield to the next thread
	uthread_yield();
}