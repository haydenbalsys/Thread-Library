#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "queue.h"

struct node
{
	void *data;
	struct node *next;
};

struct queue
{
	struct node *head;
	struct node *tail;
	int length;
};

queue_t queue_create(void)
{
	// Allocate memory for queue
	queue_t queue = malloc(sizeof(struct queue));

	// Check if malloc failed
	if (queue == NULL)
		return NULL;
	// Else, initialize queue
	queue->head = NULL;
	queue->tail = NULL;
	queue->length = 0;
	return queue;
}

int queue_destroy(queue_t queue)
{
	// Check for valid inputs
	if (queue == NULL || queue->length > 0)
		return -1;

	// Free the queue
	free(queue);
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	// Check for valid inputs
	if (queue == NULL || data == NULL)
		return -1;

	// Create new node
	struct node *newNode = malloc(sizeof(struct node));
	if (newNode == NULL)
		return -1;

	// Initialize new node
	newNode->data = data;
	newNode->next = NULL;

	// Add new node to queue
	if (queue->head == NULL)
		queue->head = newNode;
	else
		queue->tail->next = newNode;

	// Update queue
	queue->tail = newNode;
	queue->length++;

	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	// Check for valid inputs
	if (queue == NULL || data == NULL || queue->head == NULL || queue->tail == NULL || queue->length == 0)
		return -1;

	// Set head data from queue to data
	*data = queue->head->data;

	// Remove head from queue and update the queue
	struct node *temp = queue->head;
	queue->head = temp->next;
	queue->length--;
	// Free the memory used by the node
	free(temp);

	// If the queue is empty, set the tail to NULL
	if (queue->length == 0)
	{
		queue->tail = NULL;
		queue->head = NULL;
	}

	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	// Initialize pointers
	struct node *temp = queue->head;
	struct node *prev = NULL;

	// Traverse the linked list
	while (temp != NULL && temp->data != data)
	{
		prev = temp;
		temp = temp->next;
	}

	// If the data is found, delete the node
	if (temp != NULL)
	{
		// If the node is the head of the linked list
		if (prev == NULL)
			queue->head = temp->next;
		// If the node is not the head of the linked list
		else
			prev->next = temp->next;
		// Free the memory used by the node
		free(temp);
		// Decrement the length of the linked list
		queue->length--;
		return 0;
	}
	else
	{
		// If the data is not found, return an error
		return -1;
	}
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	// Check for valid inputs
	if (queue == NULL || func == NULL)
		return -1;

	// Check if queue is empty
	if (queue->length <= 0)
		return 0;

	// Call func on each node in the queue
	struct node *node;
	struct node *next = queue->head;
	// Iterate through the queue
	while (node != NULL || next != NULL)
	{
		// Call func on the node
		node = next;
		next = node->next;
		func(queue, node->data);
	}

	return 0;
}

int queue_length(queue_t queue)
{
	return queue->length;
}
