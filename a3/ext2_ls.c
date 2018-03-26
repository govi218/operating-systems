#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

int do_ls(char *ext2_disk_name, char *dir, int aFlag){
    DIR* dest_dir; 
    dest_dir = opendir(dir);
    struct dirent* dr; 

    if (dest_dir == NULL) {
        perror("No such file or directory\n");
        return ENOENT;
    }
    
    while ((dr = readdir(dest_dir)) != NULL) {
        printf("%s\n", dr->d_name);
    }

    return 0;
}

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