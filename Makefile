CC = gcc
CFLAGS = -Wall -Wextra -std=c99 $(shell sdl2-config --cflags) -I/usr/include/SDL2 $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS = $(shell sdl2-config --libs) -lm -lSDL2_image $(shell pkg-config --libs gtk+-3.0)

SRC_FILTER = filter.c
SRC_TURN = turn.c
SRC_CUT = cut.c

OBJ_FILTER = $(SRC_FILTER:.c=.o)
OBJ_TURN = $(SRC_TURN:.c=.o)
OBJ_CUT = $(SRC_CUT:.c=.o)

TARGET_FILTER = filter
TARGET_TURN = turn
TARGET_CUT = cut

all: $(TARGET_FILTER) $(TARGET_TURN) $(TARGET_CUT)

$(TARGET_FILTER): $(OBJ_FILTER)
	$(CC) $(OBJ_FILTER) -o $(TARGET_FILTER) $(LDFLAGS)

$(TARGET_TURN): $(OBJ_TURN)
	$(CC) $(OBJ_TURN) -o $(TARGET_TURN) $(LDFLAGS)

$(TARGET_CUT): $(OBJ_CUT)
	$(CC) $(OBJ_CUT) -o $(TARGET_CUT) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_FILTER) $(OBJ_TURN) $(OBJ_CUT)
	rm -f $(TARGET_FILTER) $(TARGET_TURN) $(TARGET_CUT)
	rm -f *_turned.png
	rm -f word_*.png
	rm -f letter_*_*.png
	rm -f *_filtered.png
	rm -f grid_char_*_*.png

.PHONY: all clean
