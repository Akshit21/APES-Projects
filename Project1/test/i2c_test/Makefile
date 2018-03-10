CFLAGS = -std=gnu11 -Wall -g -O0

ifeq ($(ARCH), BBB)
BBB_USER = debian
BBB_IP = 192.168.7.2
BBB_DIRECTORY = /home/debian/bin
CC = arm-linux-gnueabihf-gcc
CFLAGS += -march=armv7-a -mtune=cortex-a8 -mfpu=neon
REMOTE = $(BBB_USER)@$(BBB_IP):$(BBB_DIRECTORY)
else
CC = gcc
endif

#
# build third-party libraries
#
SUBDIRS = 3rd-party

ifdef SUBDIRS
.PHONY : $(SUBDIRS)
$(SUBDIRS) :
	@if [ -d $@ ]; then \
		$(MAKE) --no-print-directory --directory=$@ \
			CC=$(CC) CFLAGS="$(CFLAGS)" $(MAKECMDGOALS); \
	fi
endif


THIRD_PARTY_DIR = 3rd-party
CMOCKA_INCLUDE_DIR = $(THIRD_PARTY_DIR)/build-Debug/include
CMOCKA_LIBRARY = $(THIRD_PARTY_DIR)/build-Debug/lib/libcmocka.a

cmocka : $(SUBDIRS)

CFLAGS += -I $(CMOCKA_INCLUDE_DIR) -lcmocka

#
# quick and dirty build rules to demonstrate testing, not good build
# system practices.
#

#
# library
#
LIB_SRCS = \
	user_i2c.c

LIB_OBJS = $(LIB_SRCS:%.c=%.o)

LIB_HEADERS = $(LIB_SRCS:%.c=%.h)

LIB = libutils.a

#
# main executable
#
EXE_SRCS = main.c
EXE_OBJS = $(EXE_SRCS:%.c=%.o)

EXE = main.out

#
# tests
#
TEST_DLINKEDLIST_SRCS = i2c_unittest.c

TEST_LINKEDLIST_OBJS = $(TEST_LINKEDLIST_SRCS:%.c=%.o)

TEST_LINKEDLIST_EXE = i2c_unittest


#
# Generic rule to generate objects
#
%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(LIB) : $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^

all : $(LIB) $(EXE)

$(EXE) : $(EXE_OBJS) $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lm

test : $(TEST_LINKEDLIST_EXE)
	sudo ./$(TEST_LINKEDLIST_EXE)
#	sudo scp $(TEST_LINKEDLIST_EXE) $(REMOTE)

$(TEST_LINKEDLIST_EXE) : $(TEST_DLINKEDLIST_SRCS) $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -L $(CMOCKA_LIBRARY) -I $(CMOCKA_INCLUDE_DIR)

.PHONY : clean
clean : FORCE
	-rm -rf *~ *.o  $(LIB) $(EXE) *.dSYM/
	-rm -rf $(TEST_LINKEDLIST_EXE)

.PHONY : clobber
clobber : $(SUBDIRS) clean


FORCE :
