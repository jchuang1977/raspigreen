CC = gcc
CFLAGS =-I./h -lbcm2835 -lm -Wall
DEPS = 
OBJ = src/amp.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

amp: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
