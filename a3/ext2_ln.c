#include "ext2.h"
#include "ext2_helpers.h"

int do_ln(char *ext2_disk_name, char *dir1, char* dir2, int flag) {
    unsigned char *disk;    
    int fd; 
 
    fd = open(ext2_disk_name, O_RDWR);
    
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    if(disk == MAP_FAILED) {
	    perror("mmap");
	    exit(1);
    }
    struct ext2_inode* cur_inode;
    cur_inode1_parent = go_to_destination(disk, dir1); //existing 
    cur_inode2_parent = go_to_destination(disk, dir2);
    
    if (cur_inode1_parent == NULL || dir[0] != '.' || cur_inode2_parent == NULL) {
        printf("No such file or directory\n");        
        return ENOENT;
    }

    if(cur_inode1_parent->i_mode & EXT2_S_IFDIR || cur_inode2_parent->i_mode & EXT2_S_IFDIR) {
        printf("Not a regular file\n");
        return EISDIR;

    } else if (cur_inode1_parent != NULL && cur_inode2_parent != NULL) { 
        char parent_dir[256];        
        char *file_name = strrchr(dir2, '/');
        
        
        if (file_name != NULL) {
            file_name = file_name + 1;
        } else {
            file_name = dir;
        }      
        
        getParentDirectory(parent_dir, dir2);
        

        cur_inode2 = go_to_destination(disk, ".");
        
        // cur_inode2 = go_to_destination(disk, ".");
        
        struct ext2_dir_entry_2 *prev_dir_entry;
        struct ext2_dir_entry_2 *cur_dir_entry;        
        int sum_rec_len = 0; 
        while(sum_rec_len < EXT2_BLOCK_SIZE) {
            prev_dir_entry = cur_dir_entry; 
            cur_dir_entry = (struct ext2_dir_entry_2 *)(disk + ((cur_inode->i_block[i])*EXT2_BLOCK_SIZE) + sum_rec_len);        
            sum_rec_len = sum_rec_len + cur_dir_entry->rec_len;

            char buf[EXT2_NAME_LEN + 1];            
            strncpy(buf, cur_dir_entry->name, cur_dir_entry->name_len);
            buf[cur_dir_entry->name_len] = '\0';
            
            if (strcmp(buf, file_name) == 0) {
            /*
                1) Create a new directory entry, and set its file_type to either
                    symlink or hardlink depending on the mode. 
                2) Set the new files inode to be the inode of the file that you are
                    trying to link to. If it's a hardlink. 
                3) If it's a symlink then set the data of the new directory entry to 
                    be the path to the file you are trying to link to.
            */
            }
        }        
    }

    return 0;
}

int main(int argc, char **argv) {
    int ret;

    if (argc != 5 && argc != 4) {
        printf("Usage: %s <ext2 disk image name> [-s] <path to file> <path to file>\n", argv[0]);
        return 0;
    }
    
    if (argc == 5 && !(strcmp(argv[2], "-s") == 0)) {
        printf("Usage: %s <ext2 disk image name> [-s] <path to file> <path to file>\n", argv[0]);
        return 0;
    }

    if (argc == 5) {
        ret = do_ln(argv[1], argv[3], argv[4], 1);
    } else {
        ret = do_ln(argv[1], argv[2], argv[3], 0);
    }

    return ret;
}   