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

int main(int argc, char **argv) {
    int aFlag = 0;

    if (argc != 4 && argc != 3) {
        printf("Usage: %s ext2_disk_name path [-a]\n", argv[0]);
        return 0;
    }
    
    if (argc == 4 && !(strcmp(argv[3], "-a") == 0)) {
        printf("Usage: %s ext2_disk_name path [-a]\n", argv[0]);
        return 0;
    }

    if (argc == 4) {
        aFlag = 1;
    }

    int ret = do_ls(argv[1], argv[2], aFlag);

    return ret;
}