OBJS   = basictest.o stresstest.o poolix.o
BTEST  = basictest.o poolix.o
STEST  = stresstest.o poolix.o
CC     = gcc
CFLAGS = -c -Wall
LFLAGS = -pthread
INC    = src


all: $(OBJS)
	$(CC) $(BTEST) $(LFLAGS) -o basictest 
	$(CC) $(STEST) $(LFLAGS) -o stresstest

lib: poolix.o 

tests: basictest.o stresstest.o

basictest.o: test/basictest.c src/poolix.h
	$(CC) $(CFLAGS) -I$(INC) test/basictest.c 

stresstest.o: test/stresstest.c src/poolix.h
	$(CC) $(CFLAGS) -I$(INC) test/stresstest.c	

poolix.o: src/poolix.c src/poolix.h
	$(CC) $(CFLAGS) src/poolix.c

clean:
	rm *.o basictest stresstest

	
