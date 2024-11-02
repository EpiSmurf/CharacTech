# Makefile

CC = gcc
CFLAGS = -Wall -Wextra 
LDFLAGS = 
LDLIBS = -lm

EXE = solver xor

all: $(EXE)

$(foreach f, $(EXE), $(eval $(f):))

.PHONY: clean

clean:
	${RM} $(EXE)

# END
