CC     = gcc
ASAN_FLAGS = -fsanitize=address -fno-omit-frame-pointer -Wno-format-security
ASAN_LIBS = -static-libasan
CFLAGS = -Wall -Werror --std=gnu99 -g3
INC    = src

OBJS   = tplib.o
#BTEST  = basictest.o poolix.o
#STEST  = stresstest.o poolix.o

ARCH := $(shell uname)
ifneq ($(ARCH),Darwin)
  LFLAGS += -lpthread
endif

#tests: basictest.o stresstest.o
#basictest.o: test/basictest.c src/poolix.h
	#$(CC) $(CFLAGS) -I$(INC) test/basictest.c
#stresstest.o: test/stresstest.c src/poolix.h
	#$(CC) $(CFLAGS) -I$(INC) test/stresstest.c


all: $(OBJS)

lib: tplib.o

tplib.o: src/tplib.c src/tplib.h src/steque.h
	$(CC) -c -o $@ $(CFLAGS) $(ASAN_FLAGS) $<

clean:
	rm *.o
