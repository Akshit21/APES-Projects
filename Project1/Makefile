CC= arm-linux-gnueabihf-gcc
CFLAGS= -g -O0 -pthread
LFLAGS= -lm -lrt

/* Add source files here*/
SSRCS= main.c 

PROJECT1=project1

all: $(PROGRAMS)

project1: $(PROJECT1)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

.PHONY: clean

clean:
	rm -rf $(PROJECT1)
