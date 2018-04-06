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

void update_inode_bmp(unsigned char* disk, unsigned int inode_num, char mod) {
    struct ext2_super_block* sb = (struct ext2_super_block*)(disk + EXT2_BLOCK_SIZE);
    struct ext2_group_desc* gd = (struct ext2_group_desc*)(disk + 2 * EXT2_BLOCK_SIZE);
    
    char* inode_bmp = (char*) (disk + gd->bg_inode_bitmap * EXT2_BLOCK_SIZE);
    
    if(mod == 'a') {
        sb->s_free_inodes_count --;
        inode_bmp[(inode_num - 1) / 8] = inode_bmp[(inode_num - 1) / 8] | (1 << (inode_num - 1) % 8);
    } else if(mod == 'd') {
        sb->s_free_inodes_count ++;
        inode_bmp[(inode_num - 1) / 8] = inode_bmp[(inode_num - 1) / 8] & ~(1 << (inode_num - 1) % 8);
    } else {
        printf("FAILURE\n");
        exit(EXIT_FAILURE);
    }
    
}

void update_block_bmp(unsigned char* disk, unsigned int block_num, char mod) {
    struct ext2_super_block* sb = (struct ext2_super_block*)(disk + EXT2_BLOCK_SIZE);
    struct ext2_group_desc* gd = (struct ext2_group_desc*)(disk + 2 * EXT2_BLOCK_SIZE);
    
    char* block_bmp = (char*) (disk + gd->bg_block_bitmap * EXT2_BLOCK_SIZE);
    
    if(mod == 'a') {
        sb->s_free_blocks_count --;
        block_bmp[(block_num - 1) / 8] = block_bmp[(block_num - 1) / 8] | (1 << (block_num - 1) % 8);
    } else if(mod == 'd') {
        sb->s_free_blocks_count ++;
        block_bmp[(block_num - 1) / 8] = block_bmp[(block_num - 1) / 8] & ~(1 << (block_num - 1) % 8);
    } else {
        printf("FAILURE\n");
        exit(EXIT_FAILURE);
    }
    
}

/*
Searches the directory for a given filename. If it doesnt exist NULL is returned
*/
struct ext2_inode* search_for_subdirectory(unsigned char* disk, struct ext2_inode *cur_inode, char *sub_dir_name, unsigned int inode_tbl_size) {
    struct ext2_dir_entry_2 *cur_dir_entry;
    int sum_rec_len = 0; 

    while(sum_rec_len < EXT2_BLOCK_SIZE) {
        cur_dir_entry = (struct ext2_dir_entry_2 *)(disk + ((cur_inode->i_block[0])*EXT2_BLOCK_SIZE) + sum_rec_len);        
        sum_rec_len = sum_rec_len + cur_dir_entry->rec_len;
        
        char buf[EXT2_NAME_LEN + 1];
        strncpy(buf, cur_dir_entry->name, cur_dir_entry->name_len);
        buf[cur_dir_entry->name_len] = '\0';

        if (strcmp(buf, sub_dir_name) == 0) {
            return (struct ext2_inode *)(disk + inode_tbl_size*EXT2_BLOCK_SIZE + EXT2_INODE_SIZE*(cur_dir_entry->inode - 1));
        } 
    }

    return NULL;
}

/*
Traverses through the inodes to get to the path. If the path doesnt exist NULL will be returned
*/
struct ext2_inode* go_to_destination(unsigned char* disk, char *path) {
    char* cur_dir_name;
    char path_copy[10000];

    strncpy(path_copy, path, strlen(path));
    
    struct ext2_group_desc *gd = (struct ext2_group_desc *)(disk + 2*1024);

    // Get the root inode
    struct ext2_inode *cur_inode = (struct ext2_inode *) (disk + gd->bg_inode_table * EXT2_BLOCK_SIZE + EXT2_INODE_SIZE);
    
    cur_dir_name = strtok(path_copy, "/");
    while (cur_dir_name != NULL) {
        cur_inode = search_for_subdirectory(disk, cur_inode, cur_dir_name, gd->bg_inode_table);
        
        if (cur_inode == NULL) {
            return NULL;
        }
        
        cur_dir_name = strtok(NULL, "/");
    }
    
    return cur_inode;
}



/*
Get the parent directory from file path
*/
void getParentDirectory(char *parentDir, char *imgFilePath){
    int i = strlen(imgFilePath) - 1;
    while (imgFilePath[i] != '/') {
        i--;
    }
    strncpy(parentDir, imgFilePath, i - 1);

    if (i > 0){
        imgFilePath[i]='\0';
    } else{
        imgFilePath[i+1]='\0';
    }
}
