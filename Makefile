CC=gcc
CFLAGS=-O3
DCFLAGS=-g -Wall -DDBG
EXECUTABLES=ckopts

# sanhwav: is wav-file's header sane, does it match up to the physical size of the file?
ckopts: ckopts.c
	${CC} ${DCFLAGS} $^ -o $@

.PHONY: clean

clean:
	rm -f ${EXECUTABLES}
