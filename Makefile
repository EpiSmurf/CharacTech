CC = gcc
CFLAGS = -Wall -Wextra -std=c99 `sdl2-config --cflags` -I/usr/include/SDL2
LDFLAGS = `sdl2-config --libs` -lSDL2_image 

SRC = grid.c
OBJ = $(SRC:.c=.o)
TARGET = grid

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
