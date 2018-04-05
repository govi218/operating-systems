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
    
    struct ext2_dir_entry_2 *cur_dir_entry;
    int sum_rec_len = 0; 

    if (cur_inode == NULL) {
        printf("No such file or directory\n");        
        return ENOENT;
    }

    if(cur_inode->i_mode & EXT2_S_IFDIR) {
        printf("Not a regular file\n");
        return EISDIR;

    } else if (cur_inode != NULL) {
        while(sum_rec_len < EXT2_BLOCK_SIZE) {
            cur_dir_entry = (struct ext2_dir_entry_2 *)(disk + ((cur_inode->i_block[0])*EXT2_BLOCK_SIZE) + sum_rec_len);        
            sum_rec_len = sum_rec_len + cur_dir_entry->rec_len;
            
            char buf[EXT2_NAME_LEN + 1];
            
            strncpy(buf, cur_dir_entry->name, cur_dir_entry->name_len);
            buf[cur_dir_entry->name_len] = '\0';
            
            if (strcmp(buf, dir) == 0) {
               
               
                // REMOVE CODE IN HERE
                printf("%s\n", buf);                
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