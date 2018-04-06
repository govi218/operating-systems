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

    if (cur_inode == NULL) {
        printf("No such file or directory\n");        
        return ENOENT;
    }

    if(cur_inode->i_mode & EXT2_S_IFDIR) {
        printf("Not a regular file\n");
        return EISDIR;

    } else if (cur_inode != NULL) { 
        char *file_name = strrchr(dir, '/');
        char parent_dir[strlen(dir) - strlen(file_name)]; 
        
        if (file_name != NULL) {
            file_name = file_name + 1;
        }

        strncpy(parent_dir, dir, strlen(dir) - strlen(file_name));
        parent_dir[strlen(dir) - strlen(file_name)-1] = '\0';        
        printf("%s\n", parent_dir);

        cur_inode = go_to_destination(disk, parent_dir);
        
        struct ext2_dir_entry_2 *prev_dir_entry;
        struct ext2_dir_entry_2 *cur_dir_entry;        
        int sum_rec_len = 0; 
        
        while(sum_rec_len < EXT2_BLOCK_SIZE) {
            prev_dir_entry = cur_dir_entry; 
            cur_dir_entry = (struct ext2_dir_entry_2 *)(disk + ((cur_inode->i_block[0])*EXT2_BLOCK_SIZE) + sum_rec_len);        
            sum_rec_len = sum_rec_len + cur_dir_entry->rec_len;

            char buf[EXT2_NAME_LEN + 1];            
            strncpy(buf, cur_dir_entry->name, cur_dir_entry->name_len);
            buf[cur_dir_entry->name_len] = '\0';
            
            if (strcmp(buf, file_name) == 0) {
                struct ext2_inode * del_inode = (struct ext2_inode *)(disk + (1024*5) + (128*(cur_dir_entry->inode -1)));
                
                del_inode->i_links_count--;
                
                prev_dir_entry -> rec_len += cur_dir_entry -> rec_len; 
                
                break;
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