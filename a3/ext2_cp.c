#include "ext2.h"
#include "ext2_helpers.h"

int do_cp(char *ext2_disk_name, char *osdir, char *imgdir) {

    unsigned char *disk; 
    int fd;
    fd = open(ext2_disk_name, O_RDWR);
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
	    perror("mmap");
	    exit(1);
    }

    // check if file exists on os exists
    FILE fp*;
    fp = fopen(osdir, "r");
    if (fp == NULL){
        printf("File does not exist");
        return ENOENT;
    }
    // get file size
    fseek(fp, 0, SEEK_END);
    long file_length = ftell(fp);
    rewind(fp);
    fclose(fp);

    //check if parent directory exists
    char parentDir[256];
    getParentDirectory(parentDir, imgdir);
    struct ext2_inode* pd_inode;
    pd_inode = go_to_destination(disk, parentDir);
    if (pd_inode == NULL){
        printf("Parent directory does not exist\n");        
        return ENOENT;
    }

    // check if file on disk exists
    struct ext2_inode* cur_inode;
    cur_inode = go_to_destination(disk, imgdir);
    if (cur_inode == NULL) {
        printf("File on image disk does not exist\n");        
        return ENOENT;
    }

    // check if enough space to add new file

    unsigned int freeInodeNum = next_inode(disk);
    update_inode_bmp(disk, freeInodeNum, "a");

    // create new i_node for file
    struct ext2_inode *new_inode = (struct ext2_inode *)(disk + (EXT2_BLOCK_SIZE) + (EXT2_INODE_SIZE*freeInodeNum));

    new_inode -> i_mode = EXT2_S_IFREG;
    new_inode  -> i_size = file_length;
    int blocks = (file_length - 1)/(EXT2_BLOCK_SIZE) + 1
    new_inode -> i_blocks  = 2*(blocks);
    new_inode -> i_date = 0;
    new_inode -> i_links_count = 1;
    // add new blocks
    for (int i = 0; i< blocks; i++){
        
    }


    


}

int main(int argc, char **argv) {
    int ret;

    if (argc != 4) {
        printf("Usage: %s <ext2 disk image name> <path to file in native os> <absolute path to disk image>\n", argv[0]);
        return 0;
    }

    ret = do_cp(argv[1], argv[2], argv[3]);

    return ret;
}


