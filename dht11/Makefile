CC = gcc
CFLAGS =-I./h -lbcm2835 -lm -Wall
DEPS = 
OBJ = src/dht11.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

dht11: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)