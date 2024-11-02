CC = gcc
CFLAGS = -Wall -Wextra -std=c99 `sdl2-config --cflags` -I/usr/include/SDL2
LDFLAGS = `sdl2-config --libs` -lSDL2_image 

SRC_GRID = grid.c
SRC_TURN = turn.c
OBJ_GRID = $(SRC_GRID:.c=.o)
OBJ_TURN = $(SRC_TURN:.c=.o)
TARGET_GRID = grid
TARGET_TURN = turn

all: $(TARGET_GRID) $(TARGET_TURN)

$(TARGET_GRID): $(OBJ_GRID)
	$(CC) $(OBJ_GRID) -o $(TARGET_GRID) $(LDFLAGS)

$(TARGET_TURN): $(OBJ_TURN)
	$(CC) $(OBJ_TURN) -o $(TARGET_TURN) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_GRID) $(OBJ_TURN) $(TARGET_GRID) $(TARGET_TURN)
	rm -f letter_*_*.png
	rm -f *_turned.png
	rm -f word_*.png
	rm -f word_*_*.png

.PHONY: all clean
