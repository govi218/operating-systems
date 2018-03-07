SRCS = simpleloop.c matmul.c blocked.c simplelinkedlist.c
PROGS = simpleloop matmul blocked simplelinkedlist

all : $(PROGS)

$(PROGS) : % : %.c
	gcc -Wall -g -o $@ $<


traces: $(PROGS)
	./runit simpleloop
	./runit matmul 100
	./runit blocked 100 25
	./runit simplelinkedlist

.PHONY: clean
clean : 
	rm -f simpleloop matmul blocked simplelinkedlist tr-*.ref *.marker *~
