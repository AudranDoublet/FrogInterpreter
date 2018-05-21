# Simple Makefile
 
# Compilers vars
CC=gcc
CPPFLAGS= -MMD #-fsanitize=address
CFLAGS= -g -Wall -Wextra -std=c99
LDFLAGS=
LDLIBS=-lreadline -lpthread -lm -lSDL2 -lSDL2_image
 
SRC= objects/*.c parser/*.c exec/*.c modules/*.c common/strutils.c common/hashmap.c error.c main.c
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}
 
all: test

-include ${DEP}
 
test:
	${CC} ${CPPFLAGS} ${CFLAGS} ${SRC} -o test ${LDLIBS}
 
clean:
	rm -f ${DEP} ${OBJ} test test.d
 
# END Makefile
