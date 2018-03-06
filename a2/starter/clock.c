#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;
int *frame_ref;

int current_frame = 0;

/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
	int return_frame = -1;
	
	while(return_frame == -1){
		// referenced
		if (frame_ref[current_frame]){
			// dereference and move clock forward
			frame_ref[current_frame] = 0;
			current_frame++;
		}
		// not referenced 
		else {
			// reference it and move clock forward
			frame_ref[current_frame] = 1;
			// store the current frame the clock is on
			return_frame = current_frame;
			current_frame++;
		}

		//reset value so it goes back to first
		if (current_frame == memsize){
			current_frame = 0;
		}
	}

	return return_frame;
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
	frame_ref[(p->frame >> PAGE_SHIFT)] = 1;
	return;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
	// allocate adequate space for frame ref
	
	frame_ref = malloc(sizeof(int) * memsize);
	
	for(int i; i<memsize; i++){
		frame_ref[i] = 0;
	}
}
