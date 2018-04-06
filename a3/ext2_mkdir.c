#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include "ext2.h"

unsigned char *disk;

int do_mkdir(char* ext2_disk_name, char* dir) {
    unsigned char *disk;    
    int fd; 
 
    fd = open(ext2_disk_name, O_RDWR);
    
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    if(disk == MAP_FAILED) {
	    perror("mmap");
	    exit(1);
    }

    struct ext2_group_desc* gd = (struct ext2_group_desc*) (disk + 2*EXT2_BLOCK_SIZE);
    struct ext2_inode* cur_inode;
    cur_inode = go_to_destination(disk, dir);
    
    struct ext2_dir_entry_2 *cur_dir_entry;
    int sum_rec_len = 0; 

    if (cur_inode == NULL) {
        printf("No such file or directory\n");        
        return ENOENT;
    }

    if (search_for_subdirectory(disk, cur_inode, dir, gd->bg_inode_table) != NULL) {
        return EEXIST;
    }

    // assign inode and update bmp
    unsigned int new_dir_inode_num = next_inode(disk);
    update_inode_bmp(disk, new_dir_inode, 'a');

    struct ext2_inode* inode_tbl = (struct ext2_inode*) (disk + (gd->bg_inode_table * EXT2_BLOCK_SIZE));
    struct ext2_inode* new_dir_inode = &(inode_tbl[new_dir_inode_num - 1]);

    unsigned int new_dir_block_num = next_block(disk);
    update_block_bmp(disk, new_dir_block, 'a');


}

int main(int argc, char **argv) {

    if (argc != 3) {
        printf("Usage: %s ext2_disk_name path\n", argv[0]);
        return 0;
    }

    int ret = do_mkdir(argv[1], argv[2]);

    return ret;
}