# Makefile

CC = gcc
<<<<<<< HEAD
CFLAGS = -Wall -Wextra -g 
LDFLAGS = 
LDLIBS = -lm -lSDL2 -lSDL2_image
=======
CFLAGS = -Wall -Wextra 
LDFLAGS = 
LDLIBS = -lm
>>>>>>> b0a8fc2e8cace7371abc55b3c39fbb9682d63c88

EXE = solver xor

all: $(EXE)

$(foreach f, $(EXE), $(eval $(f):))

.PHONY: clean

clean:
	${RM} $(EXE)

# END
