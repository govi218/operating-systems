#include "ext2.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

unsigned int next_inode(char* disk);
unsigned int next_block(char* disk);
struct ext2_inode* search_for_subdirectory(unsigned char* disk, struct ext2_inode *cur_inode, char *sub_dir_name, unsigned int inode_tbl_size);
struct ext2_inode* go_to_destination(unsigned char* disk, char *path);