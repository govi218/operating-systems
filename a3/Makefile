FLAGS = -Wall -std=gnu99

all: ext2_ls ext2_rm ext2_cp ext2_mkdir

ext2_mkdir: ext2_mkdir.o ext2_helpers.o
	gcc ${FLAGS} -o $@ $^

ext2_rm: ext2_rm.o ext2_helpers.o
	gcc ${FLAGS} -o $@ $^

ext2_ls: ext2_ls.o ext2_helpers.o 
	gcc ${FLAGS} -o $@ $^

ext2_cp: ext2_cp.o ext2_helpers.o 
	gcc ${FLAGS} -o $@ $^

ext2_mkdir: ext2_mkdir.o ext2_helpers.o 
	gcc ${FLAGS} -o $@ $^

%.o: %.c ${DEPENDENCIES}
	gcc ${FLAGS} -c $<

clean: 
	rm *.o ext2_ls ext2_rm ext2_cp ext2_mkdir
