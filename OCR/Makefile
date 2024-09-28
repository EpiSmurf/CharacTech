# Makefile

CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS =
LDLIBS =

EXE = solver

all: $(EXE)

$(foreach f, $(EXE), $(eval $(f):))

.PHONY: clean

clean:
	${RM} $(EXE)

# END
