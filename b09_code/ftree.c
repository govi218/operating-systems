#include <stdio.h>
#include "ftree.h"
#include "hash.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <libgen.h>


void copy_file(char* src, char* dest) {
	char a;
	strcat(dest,"/");
	strcat(dest,basename(src));
	//open read and write streams
	FILE* read;
	FILE* write;

	read = fopen(src, "r");
	//chdir(dest);
	write = fopen(dest, "w");

	//error checking
	if (read == NULL) //|| (write == NULL))
	{	
		perror("Read Error: ");
		exit(0);
	}

	else if (write == NULL)
	{
		perror("Write Error: ");
		exit(0);
	}

	//write from src to dest char by char
	while (1){
    	a = fgetc(read);
    	if (a == EOF) 
    	{
    		break;
    	}
    	fputc(a, write);
   	}
 		
    struct stat src_st;

    if(fstat(fileno(read), &src_st)){
		perror("stat: ");
		exit(EXIT_FAILURE);
	}

	if(chmod(dest, src_st.st_mode & 07777)){
		perror("chmod: ");
		exit(EXIT_FAILURE);
	}
  	//close files
   	fclose(read);
   	fclose(write);
	dest = dirname(dest);

}

void hash_copy(char* src, char* dest) {
	//declarations/initialization
	FILE* src_hash;
	FILE* dest_hash;
	DIR* dest_dir;
	dest_dir = opendir(dest);
	struct dirent* dr;

	//Then extract file name and path name for later use	
	if (dest_dir == NULL)
	{
		perror("Hash Path Error: ");
		exit(EXIT_FAILURE);
	}

	//Loop through files in dest directory
	while ((dr = readdir(dest_dir)) != NULL) {

     	//if conflicting file names, compare hashes
        if (strcmp(basename(src), dr->d_name) == 0)
        {	
		    //get status for src
    		struct stat src_st;
    		struct stat dest_st;
    		char* dest_path = strdup(dest);
    		strcat(dest_path, "/");
    		strcat(dest_path, dr->d_name);
    		if (stat(src, &src_st) != 0) {
				perror("stat: ");
				exit(0);
			}

			if (stat(dest_path, &dest_st) != 0) {
				perror("stat: ");
				exit(0);
			}

        	//if the sizes match, compare hash
        	if ((src_st.st_size) == (dest_st.st_size))
        	{
            	//open conflicting file to prepare for hash

            	src_hash = fopen(src, "r");
	        	//chdir(dest);
				dest_hash = fopen(dest_path, "r");

				//error checking
				if ((dest_hash == NULL) || src_hash == NULL)
				{
					perror("Hash Path Error: ");
					exit(EXIT_FAILURE);
				}

	        	//if hashes are not the same, overwrite
   		     	if (strcmp(hash(src_hash), hash(dest_hash)) != 0)
	       	 	{
					//chdir(src_dir);
					free(dest_path);
	        		copy_file(src, dest);
	        	    fclose(dest_hash);
	        	    closedir(dest_dir);
	        	    fclose(src_hash);
	        	    return;
   		     	}

   		     	//if hashes are the same, skip
   		     	free(dest_path);
        	    fclose(dest_hash);
   		     	return;	    		
        	}

     		//if they have different sizes, overwrite
     		free(dest_path);
    		copy_file(src, dest);      
        	return;
        }
    }
    //if same file name was not found, go ahead and copy
	copy_file(src, dest);
    //close anything open
    closedir(dest_dir);
}


void copy_dir(char *src_path, char *dest_path) {
	char *file_name = basename(src_path);
	strcat(dest_path, "/");
	strcat(dest_path, file_name);
	struct stat stat_buffer;
	if (lstat(src_path, &stat_buffer) != 0) {
		perror("lstat: ");
		exit(0);
	}
	mkdir(dest_path, stat_buffer.st_mode & 07777);
	chmod(dest_path, stat_buffer.st_mode & 07777);
}

void update_dir_permissions(char *src_path, char *dest_path) {
	char *file_name = basename(src_path);
	strcat(dest_path, "/");
	strcat(dest_path, file_name);
	struct stat stat_buffer;
	if (lstat(src_path, &stat_buffer) != 0) {
		perror("stat: ");
		exit(0);

	} else {
		chmod(dest_path,stat_buffer.st_mode & 07777);
	}
}

int exists(char *dest_path, char *file_name) {
	DIR *destination = opendir(dest_path);
	struct dirent *destent;
	while ((destent = readdir(destination)) != NULL) {
		if (strcmp((*destent).d_name,file_name) == 0) {
			return 1;
		}
	}
	return 0;
}


/* dir_compare takes 2 paths:
*       src_path: is a path to the source directory
*       dest_path: is a path to the destination directory
*  
*  Case 1: Directory exists in destination
*      update permissions
*
*  Case 2: Directory does NOT exist in destination
*      create new directory
*
*/
void dir_compare(char *src_path, char *dest_path) {
	char *file_name = basename(src_path);
	if (exists(dest_path,file_name)) {
		update_dir_permissions(src_path, dest_path);
	} else {
		copy_dir(src_path, dest_path);
	}
}

int copy_ftree(const char *src, const char *dest) {
	int processes;
	int r;
	int return_status;
	char *dest_path = strdup(dest);
	char *src_path = strdup(src);

    // check if input src folder exists
    struct stat src_buff;
    if (lstat(src_path, &src_buff) != 0) {
		perror("Stat: ");
		exit(0);
	}
	if(S_ISDIR(src_buff.st_mode) == 1){
	    dir_compare(src_path,dest_path);
	} else {
		hash_copy(src_path,dest_path);
		return 0;
	}
	DIR *source = opendir(src_path); 

	// points to first directory entry INSIDE source
	struct dirent *srcent;

	while ((srcent = readdir(source)) != NULL) {
		// Skip files starting with '.'
		while ((srcent!= NULL) && (srcent->d_name[0] == '.')) {
			srcent = readdir(source); //reads next file
		}

		strcat(src_path,"/");
		strcat(src_path,srcent->d_name);
		if (stat(src_path, &src_buff)) {
			perror("stat2: ");
			break;
		}

		if(S_ISDIR(src_buff.st_mode) == 1){
			r = fork();
			if (r == 0){
				// points to current directory strea
				processes += 1;
				copy_ftree(src_path,dest_path);
				exit(processes);
			} else {    

    			wait(&return_status);
    			processes += return_status/255;
    			src_path = dirname(src_path);
			}
		} else if(S_ISREG(src_buff.st_mode)) {
			hash_copy(src_path, dest_path);
			src_path = dirname(src_path);
		}
	}
	return processes;
}