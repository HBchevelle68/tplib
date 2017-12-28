CC     = gcc
ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer -Wformat-security
ASAN_LIBS = -static-libasan
CFLAGS = -Wall -Werror --std=gnu99 -g3

OBJ = tplib.o steque.o
OBJ_NOSAN = tplib_noasan.o steque_noasan.o

ARCH := $(shell uname)
ifneq ($(ARCH),Darwin)
  LFLAGS += -lpthread
endif

# default is to build with address sanitizer enabled
all: clean basictest

lib: tplib.o steque.o

basictest: basictest.o $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(ASAN_FLAGS) $^ $(LFLAGS) $(ASAN_LIBS)

basictest_noasan: basictest_noasan.o $(OBJ_NOSAN)
	$(CC) -o $@ $(CFLAGS) $^ $(LFLAGS)

%_noasan.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $(ASAN_FLAGS) $<

clean:
		rm -fr *.o basictest basictest_noasan
