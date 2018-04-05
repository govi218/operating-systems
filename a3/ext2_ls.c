#include "ext2.h"

int do_ls(char *ext2_disk_name, char *dir, int aFlag) {
    unsigned char *disk;    
    int fd; 

    fd = open(ext2_disk_name, O_RDWR);
    
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    if(disk == MAP_FAILED) {
	    perror("mmap");
	    exit(1);
    }

    struct ext2_group_desc *sb = (struct ext2_group_desc *)(disk + 2*1024);

    printf("Inodes: %d\n", sb->bg_inode_table);
    printf("Blocks: %d\n", sb->bg_inode_bitmap);
    

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