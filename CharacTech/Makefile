CC = gcc
CFLAGS = -Wall -Wextra -O2 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_image

all: main

main: main.o
	$(CC) -o main main.o $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f main main.o
