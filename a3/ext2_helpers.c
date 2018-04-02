#include "ext2.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>


unsigned int next_inode(char* disk) {
    struct ext2_super_block* sb = (struct ext2_super_block *)(disk + 1024);
    if(sb->s_free_inodes_count > 0) {
        struct ext2_group_desc *gd = (struct ext2_group_desc *)(disk + 2*1024);
        int i, j;
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 8; j++) {
                if((*(disk+1024+(gd->bg_inode_bitmap-1)*EXT2_BLOCK_SIZE+i)>>j & 1) == 0) {
                    return 8*i + j + 1;
                }
            }
        }
    }
    return -ENOMEM;
}


unsigned int next_block(char* disk) {
    struct ext2_super_block* sb = (struct ext2_super_block *)(disk + 1024);
    if(sb->s_free_blocks_count > 0) {
        struct ext2_group_desc *gd = (struct ext2_group_desc *)(disk + 2*1024);
        int i, j;
        for (i = 0; i < 16; i++) {
            for (j = 0; j < 8; j++) {
                if((*(disk+1024+(gd->bg_block_bitmap-1)*EXT2_BLOCK_SIZE+i)>>j & 1) == 0) {
                    return 8*i + j + 1;
                }
            }
        }
    }
    return -ENOMEM;
}