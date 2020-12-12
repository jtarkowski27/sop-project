
CC = gcc
CFLAGS= -g -std=gnu99 -Wall 
LDLIBS = -lpthread -lm
DEPS = options.h
OBJ = main.o options.o file_analysis.o regex_match.o result_handler.o mistake_handler.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< $(LDLIBS)

main: $(OBJ)
	gcc $(CFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm main
	rm *.o