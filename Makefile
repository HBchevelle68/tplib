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




all: $(OBJS)

lib: tplib.o

basictest: basictest.o tplib.o


basictest.o: test/basictest.c src/tplib.h
	$(CC) $(CFLAGS) -I$(INC) $<

	#stresstest.o: test/stresstest.c src/poolix.h
		#$(CC) $(CFLAGS) -I$(INC) test/stresstest.c

tplib.o: src/tplib.c src/tplib.h src/steque.h
	$(CC) -c -o $@ $(CFLAGS) $(ASAN_FLAGS) $<

tests: basictest #stresstest.o

#%.o : %.c
#		$(CC) -c -o $@ $(CFLAGS) $(ASAN_FLAGS) $<

clean:
	rm *.o
