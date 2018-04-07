#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "ext2.h"
#include "ext2_helpers.h"

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

    char *parent_path;
    getParentDirectory(parent_path, dir);
    
    if (strlen(dir) - strlen(parent_path) > EXT2_NAME_LEN) {
        printf("Name can be max %d characters\n", EXT2_NAME_LEN);
        exit(EXIT_FAILURE);
    }

    // assign inode and block; update respective bmp
    unsigned int new_dir_inode_num = next_inode(disk);
    update_inode_bmp(disk, new_dir_inode_num, 'a');
    struct ext2_inode* inode_tbl = (struct ext2_inode*) (disk + (gd->bg_inode_table * EXT2_BLOCK_SIZE));
    struct ext2_inode* new_dir_inode = &(inode_tbl[new_dir_inode_num - 1]);
    unsigned int new_dir_block_num = next_block(disk);
    update_block_bmp(disk, new_dir_block_num, 'a');

    //set inode properties
    new_dir_inode->i_mode = EXT2_S_IFDIR;
    new_dir_inode->i_size = EXT2_BLOCK_SIZE;
    new_dir_inode->i_blocks = 2;
    new_dir_inode->i_block[0] = new_dir_block_num;
    for(int i = 1; i < 15; i++) {
        new_dir_inode->i_block[i] = 0;
    }

    //create dir entry for new dir
    struct ext2_dir_entry_2* new_dir = (struct ext2_dir_entry_2*) (disk + (new_dir_block_num * EXT2_BLOCK_SIZE));
    new_dir->inode = new_dir_inode_num;
    new_dir->file_type = EXT2_FT_DIR;
    new_dir->rec_len = sizeof(struct ext2_dir_entry_2) + 4;
    new_dir->name_len = 1;
    strncpy(new_dir->name, ".", 1);

    //create dir entry for parent
    struct ext2_dir_entry_2* parent_dir = (struct ext2_dir_entry_2*) new_dir + new_dir->rec_len;
    parent_dir->inode = cur_inode;
    parent_dir->file_type = EXT2_FT_DIR;
    parent_dir->rec_len = sizeof(struct ext2_dir_entry_2) + 4;
    parent_dir->name_len = 2;
    strncpy(new_dir->name, "..", 2);

    //get name of new dir
    char[EXT2_NAME_LEN] new_dir_name;
    int idx = strlen(parent_path) - 1, EXT2_NAME_LEN - 1;
    strncpy(new_dir_name, dir[idx], EXT2_NAME_LEN - 1);

    //create a dir record for new dir in parent
    cur_inode->i_links_count ++;
    struct ext2_dir_entry_2 *prev_dir_entry;
    struct ext2_dir_entry_2 *cur_dir_entry;        
    int sum_rec_len = 0; 
    for (int i = 0; i < 12; i++) {
        if (cur_inode->i_block[i] == 0) {
            cur_inode->i_block[i] = next_block(disk);
            update_block_bmp(disk, cur_inode->i_block[i], 'a');
            //repurposing new_dir
            new_dir = (struct ext2_dir_entry_2*) disk + (cur_inode->i_block[i] * EXT2_BLOCK_SIZE);
            new_dir->inode = new_dir_inode_num;
            new_dir->rec_len = EXT2_BLOCK_SIZE;
            new_dir->name_len = strlen(new_dir_name);
            strncpy(new_dir->name, new_dir_name, strlen(new_dir_name));
            new_dir->file_type = EXT2_FT_DIR;
            return 0;
        }
    }  
    //prob triggered by indirection
    return 1;
}

int main(int argc, char **argv) {

    if (argc != 3) {
        printf("Usage: %s ext2_disk_name path\n", argv[0]);
        return 0;
    }

    int ret = do_mkdir(argv[1], argv[2]);

    return ret;
}