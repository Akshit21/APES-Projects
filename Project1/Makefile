#CC= arm-linux-gnueabihf-gcc
CC= gcc
CFLAGS= -g -O0 -pthread
LFLAGS= -lm -lrt

SSRCS = main.c logger.c myqueue.c led.c

PROJECT1=project1

all: $(PROJECT1)

project1: $(SSRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

.PHONY: clean

clean:
	rm -rf $(PROJECT1)
