FLAGS = -Wall -std=gnu99

all: ext2_ls

ext2_ls: ext2_ls.o 
	gcc ${FLAGS} -o $@ $^

%.o: %.c ${DEPENDENCIES}
	gcc ${FLAGS} -c $<

clean: 
	rm *.o ext2_ls