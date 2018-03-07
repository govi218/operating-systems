#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"

#define MAXLINE 256

extern int memsize;

extern int debug;

extern struct frame *coremap;

extern char* tracefile;

int tf_size = 0;
addr_t *tf_list;
int current_line = 0;
int *frame_mem_ref;


/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {

	int return_frame = frame_mem_ref[0];
	int next_used = 0;
	// loop through each frame
	for(int i = 0; i<memsize;i++){
		int count = 0;
		int found = 0;
		// from current line, check when that frame is accessed again
		for (int j = current_line; j<tf_size; j++){
			count++;
			if (frame_mem_ref[i] == tf_list[j]){
				found = 1;
				break;
			}
		}
		// if frame wasn't found, then we can evict that frame
		if (!found){
			next_used = tf_size + 1;
			return_frame = frame_mem_ref[i];
		}
		// otherwise, set as the frame to return, if it is used much later
		else if (count > next_used){
			next_used = count;
			return_frame = frame_mem_ref[i];
		}
	}
	return return_frame;
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {
	//store current line address in memory frame
	frame_mem_ref[p->frame >> PAGE_SHIFT] = tf_list[current_line];
	current_line++;
	return;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {

	//loop through tracefile and store in an array

	//open tracefile
	FILE *tfp;
	if(tracefile != NULL) {
		if((tfp = fopen(tracefile, "r")) == NULL) {
			perror("Error opening tracefile:");
			exit(1);
		}
	}
	//first get size
	char buf[MAXLINE];
	while(fgets(buf, MAXLINE, tfp) != NULL) {
		if(buf[0] != '=') {
			tf_size++;
		}
	}
	
	//store elements in list
	rewind(tfp);
	tf_list = malloc(sizeof(addr_t)*tf_size);
	addr_t vaddr;
	char type;
	int i = 0;
	while(fgets(buf, MAXLINE, tfp) != NULL) {
		if(buf[0] != '=') {
			sscanf(buf, "%c %lx", &type, &vaddr);
			tf_list[i] = vaddr;
			i++;
		}
	}

	//create list for frames in memory
	frame_mem_ref = malloc(sizeof(int)*memsize);

}

