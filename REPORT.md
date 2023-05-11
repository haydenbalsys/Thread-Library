# Project 2: User-Level Threads

## Queue Implementation

Our queue is implemented using a linked list with two pointers, a head and a 
tail. The first element added to the list will be the first one out (FIFO). A 
linked list satisfied our time complexity restraints allowing us to create all 
of our functions (other than `queue_iterate()` and `queue_delete()`) in O(1) 
time.

Elements can be added to the queue by first calling `queue_create()` to 
allocate memory, then, we can add elements by calling `queue_enqueue()` which 
will set the head pointer to the new node.

Elements can be removed by using the `queue_dequeue()` function. This will 
remove the oldest item in the queue by setting the node's data pointer to its 
data. `queue_delete()` will remove the node that contains the specified data 
from the queue. Finally, to destroy the queue we call `queue_destroy()` which 
will deallocate all the memory used in by the queue.

---
## Uthread Implementation

Our uthread library has two global queues, ready_Q and zombie_Q, which hold 
threads that are ready to run and threads that have finished running. It is 
important to keep these items as global queues becaue they need to be acceed by 
all functions and we dont want to lose the data when the thread leaves the 
function. Global varible, `curr_thread` keeps track of the currently running 
thread that isnt stored in the ready queue. The `idle_thread` is used to store 
the thread that our `main` process is running, this thread is in charge of 
dealing with the zombie threads and freeing their memory. Then it will also 
wait till all other threads have completed before exiting the mulitthread 
process.

The `uthread_current()` function returns a pointer to the currently running 
thread. The `uthread_create()` function creates a new thread and adds it to the 
ready queue. The `uthread_yield()` function saves the current context and 
switches to the next ready thread in the queue. The `uthread_exit()` function 
marks the current thread as exited and switches to the next thread in the 
queue. The `uthread_block()` function sets the current thread's state to 
blocked and switches to the next thread in the queue. `uthread_unblock()` 
simply unblocks the specified thread.

---
## Semaphore Implementation

Our semaphore is implemented using a structure with two fields: count, an 
integer that represents the number of available resources, and blocked_Q, a 
queue that contains threads waiting for the semaphore. We decided to have the 
semaphore libary to take care of dealing with its own threads that are blocked 
because we can havd them wait inline for that sem to be avalible. The 
implementation satisfies our time complexity constraints, allowing us to create 
all of our 
functions (other than `sem_destroy()`) in O(1) time. This is inpart due to the 
fact that we are using a queue to store the blocked threads.

A semaphore can be created by calling `sem_create()` function. This will 
allocate memory for the semaphore structure and initialize its fields with the 
specified count. If the allocation fails, it returns NULL.

To destroy a semaphore, we call `sem_destroy()`, which checks if the semaphore 
is valid and not being used by any thread. If other threads are blocked on the 
semaphore or if its invalid, `sem_destroy()` will return -1. Otherwise, the 
function will deallocate all memory used by the semaphore. This count check 
helps prevent the user from deleting running threads that are blocked.

To take a semaphore, we call `sem_down()`. This first checks if the semaphore 
is valid. If its not, `sem_down()` will return -1. Otherwise, the function will 
attempt to decrement the semaphore's count. If the count is zero, the function 
will enqueue the calling thread onto the semaphore's waiting queue and block 
the thread. If the count is nonzero, the function will decrement the count and 
allow the thread to continue.

To release a semaphore, we call `sem_up()`. This function first checks if the 
semaphore is valid. If it is not, `sem_up()` will return -1. Otherwise, the 
function increments the semaphore's count and checks if there are any threads 
waiting on the semaphore. If there are, the function dequeues the oldest thread 
from the semaphore's waiting queue and unblocks it by calling 
`uthread_unblock()` putting the thread on the main ready queue to run on its 
turn.

---
## Preemption Implementation

The preemption mechanism in the libuthread library uses a timer to force a 
context switch between threads. The frequency of the timer is set by HZ which 
is defined as 100 times per second.

When preemption is disabled using `preempt_disable()`, the SIGVTALRM signal is 
blocked. Conversely, enabling preemption using `preempt_enable()` unblocks the 
signal. These functions are mainily used by the `uthread_yield()` function to 
ensure that that thread isnt interupted from making a context switch.

To enable preemption, `preempt_start()` sets up a timer to send the SIGVTALRM 
signal at a frequency of 100 Hz using `setitimer()`. A signal handler function 
`sighandler()` is also registered to handle the SIGVTALRM signal. The handler 
function calls `uthread_yield()` to force a context switch between threads.

To stop preemption, `preempt_stop()` restores the previous timer configuration 
using `setitimer()` and the previous signal handler function using 
`sigaction()`.

We keep two global varibles for the previous timer and signal handler to 
restore them when we stop preemption. This is important because we dont want to 
change the timer and signal handler for the whole process, just for the threads 
that are running using our thread library.

---
## Testing

### queue_tester.c

These tests are testing various functionalities of our queue data structure 
implementation. First we test that the queue is created successfully with 
`test_create()`. Next we test that we can add two items to the queue and then 
remove the first item to ensure that the first item added is also the first 
item removed (FIFO) with `test_FIFO()`. `test_queue_empty()` checks if a newly 
created queue is empty. `test_add_item()` adds an item to the queue and checks 
if the enqueue operation was successful. `test_addRemove_mult_items()` adds 
multiple items to the queue, then removes them all and ensures that the queue 
is empty. `test_queue_dequeue()` adds an item to the queue, removes it, and 
ensures that the dequeued item is the same as the one that was originally 
added. We chose these methods because they were simple and straight forward 
methods to test the functionality of the queue data structure.

### test_preempt.c

This tests whether the uthread library can handle a timer interrupt signal and 
switch to another thread correctly. The main thread creates a child thread that 
loops continuously and increments a counter variable `count`. The main thread 
also sets an interupt timer when running the multithread libary function 
`thread_run()`. The second child thread tests the functionality of the uthread 
library to perform context switching and signal handling properly. If the 
signal handling fails then the loop would increment the `count` variable 
infinitely. If context switching fails then we would not see the `count` 
variable printed from the second child. The `count` variable is used to see how 
many times the first child thread was able to loop before the second child 
thread was able to run and terminate the process. We chose this method because 
it was a simple and straight forward method to test the functionality of the 
uthread library's signal handling ability.  

