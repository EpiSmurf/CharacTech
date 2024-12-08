CC := gcc
CFLAGS := -Wall -Wextra -Werror -std=c99 $(shell sdl2-config --cflags) -I/usr/include/SDL2 $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS := $(shell sdl2-config --libs) -lm -lSDL2_image $(shell pkg-config --libs gtk+-3.0)
LDLIBS := -lSDL2 -lSDL2_image -lSDL2_ttf

TARGET_INTERFACE := interface
OBJS_INTERFACE := interface.o

SRC_FILTER := filter.c
SRC_TURN := turn.c
SRC_CUT := cut.c

OBJ_FILTER := $(SRC_FILTER:.c=.o)
OBJ_TURN := $(SRC_TURN:.c=.o)
OBJ_CUT := $(SRC_CUT:.c=.o)

TARGET_FILTER := filter
TARGET_TURN := turn
TARGET_CUT := cut

all: $(TARGET_INTERFACE) $(TARGET_FILTER) $(TARGET_TURN) $(TARGET_CUT)

$(TARGET_INTERFACE): $(OBJS_INTERFACE)
	$(CC) $(OBJS_INTERFACE) -o $(TARGET_INTERFACE) $(LDLIBS)

$(TARGET_FILTER): $(OBJ_FILTER)
	$(CC) $(OBJ_FILTER) -o $(TARGET_FILTER) $(LDFLAGS)

$(TARGET_TURN): $(OBJ_TURN)
	$(CC) $(OBJ_TURN) -o $(TARGET_TURN) $(LDFLAGS)

$(TARGET_CUT): $(OBJ_CUT)
	$(CC) $(OBJ_CUT) -o $(TARGET_CUT) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS_INTERFACE) $(OBJ_FILTER) $(OBJ_TURN) $(OBJ_CUT)
	rm -f $(TARGET_INTERFACE) $(TARGET_FILTER) $(TARGET_TURN) $(TARGET_CUT)
	rm -f *_turned.png
	rm -f word_*.png
	rm -f grid_*.png
	rm -f *_filtered.png
