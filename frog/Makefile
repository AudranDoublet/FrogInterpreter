# Simple Makefile
 
# Compilers vars
CC=gcc
CPPFLAGS=#-fsanitize=address
CFLAGS= -O4 -Wall -Wextra -std=c99
LDFLAGS=
LDLIBS=-lreadline -lpthread -lm -lSDL2 -lSDL2_image
 
SRC= $(shell find -name '*.c')
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}

all: frog

-include ${DEP}
 
frog: ${OBJ}
	${CC} -o $@ ${CPPFLAGS} ${CFLAGS} $^ ${LDLIBS}
 
clean:
	rm -f ${DEP} ${OBJ} frog frog.d
 
# END Makefile
