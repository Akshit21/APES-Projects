BBB_USER = debian
BBB_IP = 192.168.7.2
BBB_DIRECTORY = /home/debian/bin
REMOTE = $(BBB_USER)@$(BBB_IP):$(BBB_DIRECTORY)

THIRD_PARTY_DIR = ../cmocka
CMOCKA_INCLUDE_DIR = $(THIRD_PARTY_DIR)/build-Debug/include
CMOCKA_LIBRARY = $(THIRD_PARTY_DIR)/build-Debug/lib/libcmocka.a
CMOCKA_BUILD_DIR = ../cmocka/build-Debug
#
# library
#
LIB_SRCS = user_i2c.c helper.c task_apds.c task_socket.c task_tmp.c task_log.c \
          api.c led.c
LIB_OBJS = $(LIB_SRCS:%.c=%.o)
LIB_INCLUDE_DIR = ../include
LIB_HEADERS = $(LIB_SRCS:%.c=%.h)
LIB = libutils.a

#
# main executable
#
EXE_SRCS = main.c
EXE_OBJS = $(EXE_SRCS:%.c=%.o)
EXE = main.out

#
# unit tests
#
UNITTEST_SRCS = unit_test.c
UNITTEST_OBJS = $(UNITTEST_SRCS:%.c=%.o)
UNITTEST_EXE = test.out

#
# Compile options
#
CC = gcc
CFLAGS = -std=gnu11 -Wall -g -O0
CMOCKA_CFLAGS = $(CFLAGS) -I $(CMOCKA_INCLUDE_DIR)
UNITTEST_CFLAGS = $(CFLAGS) -L $(CMOCKA_LIBRARY) -I $(CMOCKA_INCLUDE_DIR) -I $(LIB_INCLUDE_DIR)
MAIN_CFLAGS = $(CFLAGS) -I $(LIB_INCLUDE_DIR)
UNITTEST_LFLAGS = -lcmocka
MAIN_LFLAGS = -pthread -lrt -lm
#
# Generic rule to generate objects
#
%.o : %.c
	$(CC) $(MAIN_CFLAGS) -c -o $@ $<

$(LIB) : $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(EXE) : $(EXE_OBJS) $(LIB)
	$(CC) $(MAIN_CFLAGS) -o $@ $^ $(MAIN_LFLAGS)

$(UNITTEST_EXE) : $(UNITTEST_SRCS) $(LIB)
	$(CC) $(UNITTEST_CFLAGS) -o $@ $^ $(UNITTEST_LFLAGS)
#
# build cmocka libraries
#
.PHONY : cmocka
cmocka :
		$(MAKE) --no-print-directory --directory=$(THIRD_PARTY_DIR) \
			CC=$(CC) CFLAGS="$(CMOCKA_CFLAGS)" $(MAKECMDGOALS);

.PHONY : all
all : $(LIB) $(EXE)
	gcc -g socket_client.c api.c -I$(LIB_INCLUDE_DIR) -o client.out $(MAIN_LFLAGS)

.PHONY : test
test : $(UNITTEST_EXE)
	sudo ./$(UNITTEST_EXE)

.PHONY : clean
clean : FORCE
	-rm -rf *~ *.o  $(LIB) $(EXE) $(UNITTEST_EXE) *.dSYM/ *.out

.PHONY : cmocka_uninstall
cmocka_uninstall :
	-rm -rf $(CMOCKA_BUILD_DIR)

FORCE :
