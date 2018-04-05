#include "ext2_helpers.h"

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

struct ext2_inode* search_for_subdirectory(unsigned char* disk, struct ext2_inode *cur_inode, char *sub_dir_name, unsigned int inode_tbl_size) {
    struct ext2_dir_entry_2 *cur_dir_entry;
    int sum_rec_len = 0; 

    while(sum_rec_len < EXT2_BLOCK_SIZE) {
        cur_dir_entry = (struct ext2_dir_entry_2 *)(disk + ((cur_inode->i_block[0])*EXT2_BLOCK_SIZE) + sum_rec_len);        
        sum_rec_len = sum_rec_len + cur_dir_entry->rec_len;
        
        if (strncmp(cur_dir_entry->name, sub_dir_name, cur_dir_entry->name_len) == 0 && strlen(cur_dir_entry->name) > 2) {
            return (struct ext2_inode *)(disk + inode_tbl_size*EXT2_BLOCK_SIZE + EXT2_INODE_SIZE*(cur_dir_entry->inode - 1));
        } 
    }

    return cur_inode;
}

struct ext2_inode* go_to_destination(unsigned char* disk, char *path) {
    char* cur_dir_name;

    struct ext2_group_desc *gd = (struct ext2_group_desc *)(disk + 2*1024);

    // Get the root inode
    struct ext2_inode *cur_inode = (struct ext2_inode *) (disk + gd->bg_inode_table * EXT2_BLOCK_SIZE + EXT2_INODE_SIZE);
    
    cur_dir_name = strtok(path, "/");
    while (cur_dir_name != NULL) {
        cur_inode = search_for_subdirectory(disk, cur_inode, cur_dir_name, gd->bg_inode_table);
        cur_dir_name = strtok(NULL, "/");
    }

    return cur_inode;
}

