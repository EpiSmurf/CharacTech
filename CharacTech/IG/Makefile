CC := gcc
CFLAGS := -Wall -Wextra -Werror
TARGET = surface
OBJS = surface.o
LDLIBS = -lSDL2 -lSDL2_image -lSDL2_ttf

all: $(TARGET)

$(TARGET): $(OBJS)

.PHONY: clean
clean:
	$(RM) $(OBJS) $(TARGET)
