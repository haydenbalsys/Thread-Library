#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"
#include "uthread.h"

struct semaphore
{
	size_t count;
	queue_t blocked_Q;
};

sem_t sem_create(size_t count)
{
	// Allocate a new sem
	sem_t new_sem = malloc(sizeof(struct semaphore));
	// Check for invalid inputs
	if (!new_sem)
		return NULL;

	// Initialize sem
	new_sem->count = count;
	new_sem->blocked_Q = queue_create();

	// Check for invalid inputs
	if (!new_sem->blocked_Q)
	{
		free(new_sem);
		return NULL;
	}

	return new_sem;
}

int sem_destroy(sem_t sem)
{
	// Check for invalid inputs
	if (sem == NULL || sem->blocked_Q != NULL || queue_length(sem->blocked_Q) != 0)
		return -1;

	// Deallocate memory
	queue_destroy(sem->blocked_Q);
	free(sem);

	return 0;
}

int sem_down(sem_t sem)
{
	if (!sem)
		return -1;

	// Wait for the semaphore to become available
	while (sem->count == 0)
	{
		queue_enqueue(sem->blocked_Q, uthread_current());
		uthread_block();
	}

	sem->count--;

	return 0;
}

int sem_up(sem_t sem)
{
	if (!sem)
		return -1;

	sem->count++;

	// Unblock the first thread in the blocked queue
	struct uthread_tcb *thread_to_unblock;
	if (!queue_dequeue(sem->blocked_Q, (void **)&thread_to_unblock))
		uthread_unblock(thread_to_unblock);

	return 0;
}