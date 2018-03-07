#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// node for doubly linked list
typedef struct node {
	int value;
	struct node* next;
	struct node* prev;
} Node;

Node* head = NULL;
Node* tail = NULL;

void printList();
void addToList(int value);
Node* findNode(int value);
void removeFromFront();
void moveToFront(int value);

void printList() {
	Node* temp = head;
	while (temp != NULL){
		printf("%d", temp->value);
		temp = temp->next;
	}
}

void addToList(int value){
	// create new node
	Node* new_node = (Node*)malloc(sizeof(Node));
	new_node->value = value;
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

Node* findNode(int value){
	Node* result = NULL;
	Node* temp = head;
	while (temp != NULL){
		if (temp->value == value){
			result = temp;
			break;
		}
		temp = temp->next;
	}
	return result;
}

void removeFromFront(){
	Node* temp = head;
	if (temp!=NULL){
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
	}
}

void moveToFront(int value){
	Node* temp = NULL;
	// look for frame in list
	temp = findNode(value);
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
}

int main(int argc, char ** argv) {
    for (int i=0; i< 1000;i++){
	addToList(i);
    }

    for (int j=0; j< 1000;j++){
	int a = rand()%3;
	if (a==0){
		int x = rand();
		addToList(x);
	} else if(a==1){
		int y = rand()%1000;
		moveToFront(y);
	} else {
		removeFromFront();
	}
	
    }
    return 0;
}
