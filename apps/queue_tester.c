#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

void test_delete(queue_t queue){
	int *data, ret, length;
	length = queue_length(queue);
	printf("%d\n", length);
	for(int i = 0; i < queue_length(queue); i++) {
		ret = queue_dequeue(queue, (void**)&data);
		if(ret != 0)
			fprintf(stderr, "---------------Failed to remove item in queue\n");
	}
	ret = queue_destroy(queue);
	if(ret != 0)
		fprintf(stderr, "---------------Failed to delete queue\n");
}

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_FIFO(void)
{
	int data = 3, data1 = 23, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_enqueue(q, &data1);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(*ptr == data);
	//test_delete(q);
}

//test case to check if a new queue is empty
void test_queue_empty(void) {
	//make sure queue is empty
	queue_t queue;
	queue= queue_create();
	fprintf(stderr, "*** TEST queue empty ***\n");
	TEST_ASSERT(queue_length(queue) == 0);
	//test_delete(queue);
}

void test_add_item(void)
{
	int data = 23;
	queue_t queue;
	fprintf(stderr, "*** TEST add item ***\n");
    queue = queue_create();
    int ret = queue_enqueue(queue, &data);
	TEST_ASSERT(ret == 0);
	//test_delete(queue);
}

void test_addRemove_mult_items() {
    queue_t queue;
    int data = 1, ret, *ptr;
	const int n = 100;
    queue = queue_create();
    fprintf(stderr, "*** TEST adding and remove mult items ***\n");

	for(int i = 0; i < n; i++) {
		ret = queue_enqueue(queue, &data);
		data++;
		if(ret != 0)
			fprintf(stderr, "Failed to queue_enqueue\n");
	}
	TEST_ASSERT(queue_length(queue) == n);
	for(int i = 0; i < n; i++) {
		ret = queue_dequeue(queue, (void**)&ptr);
		if(ret != 0)
			fprintf(stderr, "Failed to queue_dequeue\n");
	}
	TEST_ASSERT(queue_length(queue) == 0);
	//test_delete(queue);
}

void test_queue_dequeue(void){
	queue_t queue;
	int data = 1, ret, *ptr;

	queue = queue_create();
	fprintf(stderr, "*** TEST queue dequeue ***\n");
	ret = queue_enqueue(queue, &data);
	if(ret != 0)
		fprintf(stderr, "Failed to queue_enqueue\n");
	ret = queue_dequeue(queue, (void**)&ptr);
	if(ret != 0)
		fprintf(stderr, "Failed to queue_dequeue\n");
	TEST_ASSERT(data == *ptr);
	//test_delete(queue);
}


int main(void)
{
	test_create();
	test_FIFO();
	test_queue_empty();
	test_add_item();
	test_addRemove_mult_items();
	test_queue_dequeue();

	return 0;
}
