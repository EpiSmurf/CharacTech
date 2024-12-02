# Makefile

CC = gcc
CFLAGS = -Wall -Wextra 
LDFLAGS = 
LDLIBS = -lm -lSDL2 -lSDL2_image

EXE = solver xor

all: $(EXE)

$(foreach f, $(EXE), $(eval $(f):))

.PHONY: clean

clean:
	${RM} $(EXE)

# END
