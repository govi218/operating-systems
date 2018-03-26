# Copy Files and Directories in C
A group of C programs that copy a directory or a file from a provided source to a provided target. This program creates a child process for each subdirectory within source which then recursively populates the directory. If there is a conflicting filename in the destination, check if sizes are different. If so, overwrite but if not, perform a hash comparison using the method in hash_functions.c. If the hashes match, do nothing but if not, overwrite. 
## Installation
Use any C compiler to compile fcopy.c, ftree.c and hash_functions.c. It is essential for all three of these files to be compiled into an executable for proper functionality.
With GCC: `$gcc -o fcopy ftree.c fcopy.c hash_functions.c`
## Usage
Run executable after compiling.
`fcopy SRC DEST`
