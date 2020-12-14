CC = gcc
CFLAGS= -g -std=gnu99 -Wall 
LDLIBS = -lpthread -lm

all: 
	$(CC) $(CFLAGS) -o prog prog.c $(LDLIBS)

clean:
	rm prog
	# rm *.o