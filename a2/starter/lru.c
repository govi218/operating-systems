#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

// node for doubly linked list
typedef struct Node {
	unsigned int frame;
	struct Node* next;
	struct Node* prev;
} Node;

Node* head = NULL;
Node* tail = NULL;

void printList();
void addToList(int frame);
Node* findNode(int frame);

void printList() {
	Node* temp = head;
	printf("start of list\n");
	while (temp != NULL){
		printf("%d->", temp->frame);
		temp = temp->next;
	}
	printf("end of list\n");
}

void printReverseList() {
	Node* temp = tail;
	printf("start reverse of list\n");
	while (temp != NULL){
		printf("%d->", temp->frame);
		temp = temp->prev;
	}
	printf("end of list\n");
}

void addToList(int frame){
	// create new node
	Node* new_node = (Node*)malloc(sizeof(Node));
	new_node->frame = frame;
	new_node->next = NULL;
	new_node->prev = NULL;
	// if head is null, set as head
	if (head == NULL){
		head = new_node;
		tail = new_node;
	}
	// else add to front
	else {
		head->prev = new_node;
		new_node->next = head;
		new_node->prev = NULL;
		head = new_node;
	}
}

Node* findNode(int frame){
	Node* result = NULL;
	Node* temp = head;
	while (temp != NULL){
		if (temp->frame == frame){
			result = temp;
			break;
		}
		temp = temp->next;
	}
	return result;
}


/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {

	// assuming list isn't empty
	// get the least recent frame from tail
	Node* temp = tail;
	int return_frame = temp->frame;
	
	// if its also the head, meaning only 1 item in list
	if (temp->prev == NULL){
		// set to null
		head = NULL;
		tail = NULL;
	}
	// else more than 1 item in list 
	else {
		// remove the temp node from tail
		tail = temp->prev;
		tail->next = NULL;
	}
	free(temp);

	return return_frame;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	
	Node* temp = NULL;
	// look for frame in list
	temp = findNode(p->frame >> PAGE_SHIFT);
	// if frame was added already
	if (temp != NULL){
		// move to front of stack, if the its not already at front of list
		if (temp->prev != NULL){
			// if the node is not the tail, move to front normally
			if(temp->next != NULL){
				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
				temp->next = head;
				temp->prev = NULL;
				head->prev = temp;
				head = temp;
			}
			// else its a tail, so move accordingly
			else {
				tail = temp->prev;
				temp->next = head;
				temp->prev = NULL;
				head->prev = temp;
				tail->next = NULL;
				head = temp;
			}
		}
	}
	// else frame wasn't added yet
	else {
		// add to front of list
		addToList(p->frame >> PAGE_SHIFT);
	}

	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
}
