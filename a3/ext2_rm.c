#include "ext2.h"
#include "ext2_helpers.h"

int do_rm(char *ext2_disk_name, char *dir) {
    unsigned char *disk;    
    int fd; 
 
    fd = open(ext2_disk_name, O_RDWR);
    
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    if(disk == MAP_FAILED) {
	    perror("mmap");
	    exit(1);
    }

    struct ext2_inode* cur_inode;
    cur_inode = go_to_destination(disk, dir);
    
    if (cur_inode == NULL || dir[0] != '.') {
        printf("No such file or directory\n");        
        return ENOENT;
    }

    if(cur_inode->i_mode & EXT2_S_IFDIR) {
        printf("Not a regular file\n");
        return EISDIR;

    } else if (cur_inode != NULL) { 
        char parent_dir[256];        
        char *file_name = strrchr(dir, '/');
        
        
        if (file_name != NULL) {
            file_name = file_name + 1;
        } else {
            file_name = dir;
        }      
        
        getParentDirectory(parent_dir, dir);
        

        cur_inode = go_to_destination(disk, ".");
        
        struct ext2_dir_entry_2 *prev_dir_entry;
        struct ext2_dir_entry_2 *cur_dir_entry;        
        int sum_rec_len = 0; 
        for (int i = 0; i < 12; i++) {
            if (cur_inode->i_block[i] == 0) {
                break; 
            }
            while(sum_rec_len < EXT2_BLOCK_SIZE) {
                prev_dir_entry = cur_dir_entry; 
                cur_dir_entry = (struct ext2_dir_entry_2 *)(disk + ((cur_inode->i_block[i])*EXT2_BLOCK_SIZE) + sum_rec_len);        
                sum_rec_len = sum_rec_len + cur_dir_entry->rec_len;
    
                char buf[EXT2_NAME_LEN + 1];            
                strncpy(buf, cur_dir_entry->name, cur_dir_entry->name_len);
                buf[cur_dir_entry->name_len] = '\0';
                
                if (strcmp(buf, file_name) == 0) {
                    struct ext2_inode * del_inode = (struct ext2_inode *)(disk + (1024*5) + (128*(cur_dir_entry->inode -1)));
                    
                    update_inode_bmp(disk, cur_dir_entry->inode,'d');
                    del_inode->i_links_count--;

                    if (del_inode->i_links_count == 0) {
                       update_block_bmp(disk, cur_inode->i_block[i],'d');
                    }
                    
                    prev_dir_entry -> rec_len += cur_dir_entry -> rec_len; 
                    break;
                }
            }
        }        
    }
    
    return 0;
}

int main(int argc, char **argv) {
    int ret;

    if (argc != 3) {
        printf("Usage: %s <ext2 disk image name> <path to file>\n", argv[0]);
        return 0;
    }
    
    ret = do_rm(argv[1], argv[2]);

    return ret;
}   