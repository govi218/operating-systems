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
typedef struct node {
	unsigned int frame;
	struct node* next;
	struct node* prev;
} Node;

Node* head = NULL;
Node* tail = NULL;

void printList();
void addToList(int frame);
Node* findNode(int frame);

void printList() {
	Node* temp = head;
	while (temp != NULL){
		printf("%d", temp->frame);
		temp = temp->next;
	}
}

void addToList(int frame){
	// create new node
	Node* new_node = (Node*)malloc(sizeof(Node));
	new_node->frame = frame;
	// if head is null, set as head
	if (head == NULL){
		head = new_node;
		head->next = NULL;
		head->prev = NULL;
		tail = new_node;
		tail->next = NULL;
		tail->prev = NULL;
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
	
	// is it possible to evict a frame if it hasn't been referenced yet??
	//if (head != NULL)

	// assuming list isn't empty
	// get the most recent frame from head
	Node* temp = head;
	int return_frame = temp->frame;

	// if its not the tail
	if (temp->next != NULL){
		// remove the temp node from head
		head = temp->next;
		head->prev = NULL;
	}
	// else its the tail 
	else {
		// set to null
		head = NULL;
		tail = NULL;
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
			// if the nodes is not the tail, move to front normally
			if(temp->next != NULL){
				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
				temp->next = head;
				temp->prev = NULL;
				head = temp;
			}
			// else its a tail, so move accordingly
			else {
				tail = temp->prev;
				temp->next = head;
				temp->prev = NULL;
				head->prev = temp;
				head->next = NULL;
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
