#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include "ext2.h"

unsigned char *disk;

int do_mkdir(char* disk_name, char* path) {
    int fd = open(disk_name, O_RDWR);
    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk = MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }
    
    

}

int main(int argc, char **argv) {

    if (argc != 3) {
        printf("Usage: %s ext2_disk_name path\n", argv[0]);
        return 0;
    }

    int ret = do_mkdir(argv[1], argv[2]);

    return ret;
}