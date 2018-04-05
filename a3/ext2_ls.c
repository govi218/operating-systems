#include "ext2.h"
#include "ext2_helpers.h"

int do_ls(char *ext2_disk_name, char *dir, int aFlag) {
    unsigned char *disk;    
    int fd; 
    char dir_copy[10000];

    strncpy(dir_copy, dir, strlen(dir));
    fd = open(ext2_disk_name, O_RDWR);
    
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    if(disk == MAP_FAILED) {
	    perror("mmap");
	    exit(1);
    }

   // struct ext2_group_desc *gd = (struct ext2_group_desc *)(disk + 2*1024);
    struct ext2_inode* cur_inode;
    cur_inode = go_to_destination(disk, dir);
    
    struct ext2_dir_entry_2 *cur_dir_entry;
    int sum_rec_len = 0; 

    if(cur_inode->i_mode & EXT2_S_IFDIR) {
        while(sum_rec_len < EXT2_BLOCK_SIZE) {
            cur_dir_entry = (struct ext2_dir_entry_2 *)(disk + ((cur_inode->i_block[0])*EXT2_BLOCK_SIZE) + sum_rec_len);        
            sum_rec_len = sum_rec_len + cur_dir_entry->rec_len;
            char buf[EXT2_NAME_LEN + 1];
            strncpy(buf, cur_dir_entry->name, cur_dir_entry->name_len);
            buf[cur_dir_entry->name_len] = '\0';
            printf("%s\n", buf);
        }
    } else if (cur_inode != NULL) {
        printf("%s\n", dir_copy);
    }


    return 0;
}

int main(int argc, char **argv) {
    int aFlag = 0;

    if (argc != 4 && argc != 3) {
        printf("Usage: %s <ext2 disk image name> <path to file> [-a]\n", argv[0]);
        return 0;
    }
    
    if (argc == 4 && !(strcmp(argv[3], "-a") == 0)) {
        printf("Usage: %s <ext2 disk image name> <path to file> [-a]\n", argv[0]);
        return 0;
    }

    if (argc == 4) {
        aFlag = 1;
    }

    int ret = do_ls(argv[1], argv[2], aFlag);

    return ret;
}   