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
//ucontext_t a;
//unsigned int cur = 0;

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
	new_thread->blocking_tid = -1;
	new_thread->ctx->uc_link = gen_die_ctx(); // you'll need to implement this method!
	tmp->t = new_thread;

	insert(tmp); // Insert node into linked list for scheduler

	return tmp;
}


void diecontext(){

    struct uthread_t * tmpthread1;
    struct uthread_t * tmpthread2;
    node* tmpnode1;
    node* tmpnode2;
    tmpnode1=get_thread_node(0);
    tmpthread1 = tmpnode1->t;

    //check for blocking not done yet
    int blockid = tmpthread1->blocking_tid;
    printf("blockid %i\n", blockid);
    if(blockid>-1)
      {
	tmpthread2=get_thread(blockid);
      }
    else
      {
	tmpnode2= get_next_ready(tmpnode1);
	tmpthread2 = tmpnode2->t;
      }
    //else if no blocking ones get next active



    tmpthread1->state = T_DEAD;
    tmpthread2->state = T_ACTIVE;
    ucontext_t* tmpcontext1;
    ucontext_t* tmpcontext2;
    tmpcontext1 = tmpthread1->ctx;
    tmpcontext2 = tmpthread2->ctx;
    swapcontext(tmpcontext1,tmpcontext2);

}

uthread_t *uthread_create(void *(*func)(void *), void *argp)
{

  //create a new context
  ucontext_t *tmpcontext= malloc(sizeof(ucontext_t));
  getcontext(tmpcontext);

 
  tmpcontext->uc_stack.ss_sp=malloc(MEM);
  tmpcontext->uc_stack.ss_size=MEM;
  tmpcontext->uc_stack.ss_flags=0;
  makecontext(tmpcontext, (void*)func,sizeof(argp), argp);


  struct uthread_t * tmpthread1;
  
  tmpthread1 = create_node(tmpcontext)->t;

  //switch from active to newly created

  struct uthread_t * tmpmainthread;

  node* tmpmainnode;

  tmpmainnode=get_thread_node(0);
  tmpmainthread = tmpmainnode->t;

  tmpmainthread->state = T_READY;
  tmpthread1->state = T_ACTIVE;
  
  ucontext_t* tmpcontext1;
  ucontext_t* tmpcontext2;

  tmpcontext1 = tmpmainthread->ctx;
  tmpcontext2 = tmpthread1->ctx;


  swapcontext(tmpcontext1,tmpcontext2);
  
  return(tmpthread1);
  

  
	// Create a new node, with a new thread, and a new context
  
}

void uthread_init(void){
	// Implement the necessary structs and state so that the first call to create above is successful
	// Hint, you can assume that this method is called by the program's main / original thread.
  //ucontext_t tmpcontext;
  ucontext_t *tmpcontext= malloc(sizeof(ucontext_t));

  getcontext(tmpcontext);
  create_node(tmpcontext);
  //node* tmpnode;
  struct uthread_t * tmpthread1;
  tmpthread1 =get_thread(0);
  tmpthread1->state = T_ACTIVE;
  


}

int uthread_get_id(void){
	// Search through queue for the active thread and return it's tid
  //printf("here!\n");
  node* tmpnode;



  tmpnode =get_thread_node(0);
  struct uthread_t * tmpthread1;
  tmpthread1 = tmpnode->t;
  unsigned int tmpint = tmpthread1->tid;
  return(tmpint);
    
}

ucontext_t *gen_die_ctx(){

  //get a context that runs die to be put as threads uc_link
  ucontext_t *tmpcontext2= malloc(sizeof(ucontext_t));
  getcontext(tmpcontext2);

  tmpcontext2->uc_link= 0;
  tmpcontext2->uc_stack.ss_sp=malloc(MEM);
  tmpcontext2->uc_stack.ss_size=MEM;
  tmpcontext2->uc_stack.ss_flags=0;
  makecontext(tmpcontext2,  (void*)&diecontext,0);


  return(tmpcontext2);
}


int uthread_yield(void){
	// Just schedule some other thread!

  struct uthread_t * tmpthread1;
  struct uthread_t * tmpthread2;
  node* tmpnode1;
    
  node* tmpnode2;


  tmpnode1=get_thread_node(0);
  tmpthread1 = tmpnode1->t;

  //gets next thread ready
  tmpnode2= get_next_ready(tmpnode1);
  tmpthread2 = tmpnode2->t;

  tmpthread1->state = T_READY;
  tmpthread2->state = T_ACTIVE;
  ucontext_t* tmpcontext1;
  ucontext_t* tmpcontext2;

  tmpcontext1 = tmpthread1->ctx;
  tmpcontext2 = tmpthread2->ctx;

  ucontext_t tmpcontext11;
  ucontext_t tmpcontext22;
  tmpcontext11 = *tmpcontext1;
  tmpcontext22 = *tmpcontext2;
  
  return(swapcontext(tmpcontext1,tmpcontext2));
  
}


int uthread_join (uthread_t thread){
  
	// Go find the given thread in the queue, tell it that it is blocking this (active) thread
	// Mark the active thread as blocked
	// Mark the given thread (from the queue) as active
	// swap the context to start the given thread (from the queue)


    struct uthread_t * tmpthread1;
    struct uthread_t * tmpthread2;

    struct uthread_t * tmpthreadtmp = &thread;
    
    node* tmpnode1;
    unsigned int tmptid = tmpthreadtmp->tid;
    tmpthread2 =get_thread(tmptid);
    
    tmpnode1=get_thread_node(0);
    tmpthread1 = tmpnode1->t;
    //doesnt let a dead thread block
    if(tmpthread2->state == T_DEAD){
      return(0);
    }

    //block and switch active thread
    tmpthread1->state = T_BLOCKED;
    tmpthread2->state = T_ACTIVE;
    tmpthread2->blocking_tid = tmpthread1->tid;
  
    ucontext_t* tmpcontext1;
    ucontext_t* tmpcontext2;

    tmpcontext1 = tmpthread1->ctx;
    tmpcontext2 = tmpthread2->ctx;

    swapcontext(tmpcontext1,tmpcontext2);
    
    return 0;
}
