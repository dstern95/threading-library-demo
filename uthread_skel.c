// User-space thread library


// Of course, my header file!
#include "uthread.h"

#include <stdlib.h> // malloc and other things
#include <stdio.h> // some debug printing


// Number of bytes in a Mebibyte, used for thread stack allocation
#define MEM 1048567


// The threads are managed with a double-linked list
static int num_threads = 0; // This is also used to count the number of linked list nodes and give thread_ids
static ucontext_t *parent_ctx;
static ucontext_t *sched_ctx;

// Inserts a node (at the back) into the circular queue
void insert(node *new_node){
	if(head == NULL){
		head = new_node;
		tail = new_node;
		//printf("head tid: %u\n", head->t->tid);
	} else {
		tail->next = new_node;
		tail = new_node;
	}
	tail->next = head; // circular
}


void print_queue(){
	int counter = 0;
	printf("-- Linked List of Threads --\n");
	node *cur = head;
	while(counter < num_threads){
		printf("Node %d with tid %u  state: %d\n", counter, cur->t->tid, cur->t->state);
		cur->visited = 1;
		cur = cur->next;
		counter++;
	}
}

node *get_thread_node(int state){
	node *cur = head;
	int counter = 0;
	while(cur->t->state != state){
		//printf("thread tid: %u state %d\n", cur->t->tid, cur->t->state);
		cur = cur->next;
		counter++;
		if(counter > num_threads){
			return NULL;
		}
	}
	return cur;
}

uthread_t *get_thread(unsigned int tid){
	//printf("Looking for thread with TID %u\n", tid);
	node *cur = head;
	int counter = 0;
	while(cur->t->tid != tid){
		cur = cur->next;
		counter++;
		if(counter > num_threads){
			return NULL;
		}
	}
	return cur->t;
}

node *get_next_ready(node *cur){
	cur = cur->next;
	int counter = 0;
	while(cur->t->state != T_READY){
		cur = cur->next;
		counter++;
		if(counter > num_threads){
			return NULL;
		}
	}
	return cur;
}



// Creates a circular queue node
node *create_node(ucontext_t *new_ctx){
	node *tmp = (node *) malloc(sizeof(node));
	tmp->next = NULL;

	uthread_t *new_thread = (uthread_t *)malloc(sizeof(uthread_t));
	
	// There is now one more thread (e.g. there was 0, now there is 1)
	// But, the TID of this thread is 0 (not one).  So, increment first, 
	// then subtract 1 to get TID.  This way, there value of num_threads is never wrong
	num_threads++;
	new_thread->tid = num_threads - 1;
	new_thread->ctx = new_ctx;
	new_thread->state = T_READY; // ready to run, but not running right now

	new_thread->ctx->uc_link = gen_die_ctx(); // you'll need to implement this method!
	tmp->t = new_thread;

	insert(tmp); // Insert node into linked list for scheduler

	return tmp;
}

uthread_t *uthread_create(void *(*func)(void *), void *argp){
	// Create a new node, with a new thread, and a new context
}

void uthread_init(void){
	// Implement the necessary structs and state so that the first call to create above is successful
	// Hint, you can assume that this method is called by the program's main / original thread.
}

int uthread_get_id(void){
	// Search through queue for the active thread and return it's tid
}


int uthread_yield(void){
	// Just schedule some other thread!
}


int uthread_join (uthread_t thread){
	// Go find the given thread in the queue, tell it that it is blocking this (active) thread
	// Mark the active thread as blocked
	// Mark the given thread (from the queue) as active
	// swap the context to start the given thread (from the queue)
	return 0;
}