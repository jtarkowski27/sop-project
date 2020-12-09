
CC = gcc
CFLAGS= -std=gnu99 -Wall
LDLIBS = -lpthread -lm
DEPS = options.h
OBJ = main.o options.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< $(LDLIBS)

main: $(OBJ)
	gcc $(CFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm main
	rm *.o